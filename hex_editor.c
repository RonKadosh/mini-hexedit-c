#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_INPUT_LEN 256
#define MAX_FILENAME_LEN 100
#define MAX_DATA_BYTES_TO_LOAD 10000
#define MENU_LOWER_BOUND 0
#define MENU_UPPER_BOUND 9



typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  char display_flag;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

state* init_state(){
    state* s = (state*)malloc(sizeof(state));
    s->debug_mode = 0;
    s->file_name[0] = '\0';
    s->unit_size = 1;
    s->mem_buf[0] = '\0';
    s->mem_count = 0;
    s->display_flag = 0;
    return s;
}

void print_state(state *s){
    fprintf(stderr, "\nDebug info:\nUnit Size: %d\nFile Name: %s\nMem Count: %d\n\n", s->unit_size, s->file_name, s->mem_count);
}

void print_decimal_1byte(char* buffer) {
    printf("%d\n", (unsigned char)buffer[0]); 
}

void print_decimal_2bytes(char* buffer) {
    printf("%d\n", (unsigned char)buffer[0] | ((unsigned char)buffer[1] << 8));
}

void print_decimal_4bytes(char* buffer) {
    printf("%u\n", (unsigned char)buffer[0] | ((unsigned char)buffer[1] << 8) | ((unsigned char)buffer[2] << 16) | ((unsigned char)buffer[3] << 24));
}

void print_hex_1byte(char* buffer) {
    printf("0x%02x\n", (unsigned char)buffer[0]);
}

void print_hex_2bytes(char* buffer) {
    printf("0x%04x\n", (unsigned char)buffer[1] | ((unsigned char)buffer[0] << 8));
}

void print_hex_4bytes(char* buffer) {
    printf("0x%08x\n", (unsigned char)buffer[3] | ((unsigned char)buffer[2] << 8) | ((unsigned char)buffer[1] << 16) | ((unsigned char)buffer[0] << 24));
}

struct PrintFunc{
  void(*fun)(char*);
};

struct PrintFunc printbytes[] = {
    {print_decimal_1byte},
    {print_decimal_2bytes},
    {NULL},
    {print_decimal_4bytes},
    {print_hex_1byte},
    {print_hex_2bytes},
    {NULL},
    {print_hex_4bytes}
};

void print_units(char* buffer, char hex_or_dec, int unit_size, int count){
    for(int i = 0; i < count * unit_size; i = i + unit_size){
        printbytes[(hex_or_dec * 4) + (unit_size - 1)].fun((buffer + i));
    }
}

void toggle_dbg_mode(state* s){
    if(s->debug_mode){
        s->debug_mode = 0;
        fprintf(stderr, "Debug flag now off.\n");
    }
    else{
        s->debug_mode = 1;
        fprintf(stderr, "Debug flag now on.\n");
    }
}

void set_file_name(state* s){
    char filename_buffer[MAX_FILENAME_LEN];
    printf("Enter a file name: ");
    fgets(filename_buffer, MAX_FILENAME_LEN, stdin);
    strcpy(s->file_name, filename_buffer);
    s->file_name[strlen(s->file_name) - 1] = '\0'; 
    if(s->debug_mode) fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
}

void set_unit_size(state* s){
    char unit_size_buffer[MAX_INPUT_LEN];
    int unit_size;
    printf("Enter a unit size: ");
    fgets(unit_size_buffer, MAX_INPUT_LEN, stdin);
    unit_size_buffer[strlen(unit_size_buffer) - 1] = '\0'; 
    unit_size = atoi(unit_size_buffer);
    if (unit_size != 1 && unit_size != 2 && unit_size != 4) fprintf(stderr, "Invalid value: unit size has to be 1, 2 or 4! size remains unchanged.\n");
    else{
        s->unit_size = unit_size;
        if(s-> debug_mode) fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
    }
}

void load_into_memory(state* s){
    FILE* file_to_load;
    char input_buffer[MAX_INPUT_LEN];
    int location;
    int length;
    if(s->file_name[0] == '\0'){
        fprintf(stderr, "File name is not initialized!\n");
        return;
    }
    file_to_load = fopen(s->file_name, "r");
    if (!file_to_load){
        fprintf(stderr, "Error: Could not open file with file name: %s\n", s->file_name);
        return;
    }
    printf("Please enter <location> <length>: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    input_buffer[strlen(input_buffer) - 1] = '\0';
    sscanf(input_buffer, "%x %d", &location, &length);
    if(s->debug_mode) fprintf(stderr, "Debug: Loading %d units from file: %s, in location: %x.\n", length, s->file_name, location);
    fseek(file_to_load, location, SEEK_SET);
    s->mem_count = fread(s->mem_buf, s->unit_size, length * s->unit_size, file_to_load);
    s->mem_buf[s->mem_count] = '\0';
    printf("Loaded %d units into memory.", length);
    fclose(file_to_load);
}

void toggle_display_mode(state* s){
    if(s->display_flag){
        s->display_flag = 0;
        fprintf(stderr, "Display flag now off, decimal representation.\n");
    }
    else{
        s->display_flag = 1;
        fprintf(stderr, "Display flag now on, hexadecimal representation.\n");
    }
}

void file_display(state* s){
    char input_buffer[MAX_INPUT_LEN];
    int file_offset;
    int length;
    FILE* file_to_display;
    char read_buffer[MAX_DATA_BYTES_TO_LOAD];
    printf("Enter <file-offset> <length>: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    input_buffer[strlen(input_buffer) - 1] = '\0';
    sscanf(input_buffer, "%x %d", &file_offset, &length);
    file_to_display = fopen(s->file_name, "r");
    if (!file_to_display){
        fprintf(stderr, "Error: Could not open file with file name: %s\n", s->file_name);
        return;
    }
    fseek(file_to_display, file_offset, SEEK_SET);
    fread(read_buffer, s->unit_size, length, file_to_display);
    printf("%s\n=======\n", (s->display_flag ? "Hexadecimal" : "Decimal"));
    print_units(read_buffer, s->display_flag, s->unit_size, length);
    fclose(file_to_display);
}

void memory_display(state* s){
    char input_buffer[MAX_INPUT_LEN];
    int address;
    int length;
    printf("Enter <address> <length>: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    input_buffer[strlen(input_buffer) - 1] = '\0';
    sscanf(input_buffer, "%x %d", &address, &length);
    printf("%s\n=======\n", (s->display_flag ? "Hexadecimal" : "Decimal"));
    print_units((char*)(s->mem_buf + address), s->display_flag, s->unit_size, length);
}

void save_into_file(state* s) {
    char input_buffer[MAX_INPUT_LEN];
    unsigned int source_address;
    int target_location;
    int length;
    FILE* file_to_save;
    printf("Enter <source-address> <target-location> <length>: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%x %x %u", &source_address, &target_location, &length);
    file_to_save = fopen(s->file_name, "r+b");
    if (!file_to_save) {
        fprintf(stderr, "Error: Could not open file with file name: %s\n", s->file_name);
        return;
    }
    unsigned int bytes_to_write = length * s->unit_size;
    unsigned char* source_ptr = (source_address == 0) ? s->mem_buf : s->mem_buf + source_address;
    if (fseek(file_to_save, target_location, SEEK_SET) != 0) {
        perror("Error seeking in file");
        fclose(file_to_save);
        return;
    }
    size_t bytes_written = fwrite(source_ptr, 1, bytes_to_write, file_to_save);
    if (bytes_written < bytes_to_write) {
        printf("Error: Only wrote %zu of %u requested bytes\n", bytes_written,
        bytes_to_write);
    } else {
        printf("Successfully wrote %u bytes to %s at position %x\n", bytes_to_write, s->file_name, target_location);
    }
    if (s->debug_mode) {
    fprintf(stderr, "Debug: Written %u bytes to %s at %x from memory %x\n", bytes_to_write, s->file_name, target_location, source_address);
    }
    fclose(file_to_save);
}

void memory_modify(state* s){
    unsigned int location;
    unsigned int val;
    printf("Please enter <location> <val>: ");
    scanf("%x %x", &location, &val);
    if (s->debug_mode) {
        fprintf(stderr, "Debug: location=0x%x, val=0x%x\n", location, val);
    }
    memcpy(s->mem_buf + location, &val, s->unit_size);
}

void quit(state* s){
    free(s);
    exit(0);
}

struct fun_desc{
  char* name;
  void(*fun)(state*);
};

struct fun_desc menu[] = { 
    { "Toggle Debug Mode", toggle_dbg_mode }, 
    { "Set File Name", set_file_name }, 
    { "Set Unit Size", set_unit_size }, 
    { "Load Into Memory", load_into_memory }, 
    { "Toggle Display Mode", toggle_display_mode }, 
    { "File Display", file_display }, 
    { "Memory Display", memory_display },
    { "Save Into File", save_into_file },
    { "Memory Modify", memory_modify },
    { "Quit", quit },
    { NULL, NULL } }; 

void print_menu(){
    int i = 0;
    printf("Choose Action:\n");
    while((menu + i)->name != NULL){
        printf("%d) %s-\n", i, (menu + i) -> name);
        i++;
    }
}


int main(int argc, char** argv){

    char input_buffer[MAX_INPUT_LEN];
    int menu_choice;
    
    state* c_state = init_state();
    while(1){
        if(c_state->debug_mode) print_state(c_state);
        print_menu();
        fgets(input_buffer, MAX_INPUT_LEN, stdin);
        if (strlen(input_buffer) != 2) continue;
        menu_choice = atoi(input_buffer);
        if (!(MENU_LOWER_BOUND <= menu_choice && menu_choice <= MENU_UPPER_BOUND)) continue;
        menu[menu_choice].fun(c_state);
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define CODE_LENGTH 9

typedef struct {
    char code[CODE_LENGTH];
    int price;
} Product;

/* Helper & Validation Functions */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int is_code_exists(Product products[], int product_count, const char *code) {
    for (int i = 0; i < product_count; i++) {
        if (strcmp(products[i].code, code) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Function Prototypes */
int load_from_file(Product products[]);
void save_to_main_file(Product products[], int product_count);
void initialize_mock_data(Product products[], int *product_count);

void swap_products(Product *a, Product *b);
int partition_by_price(Product products[], int low, int high);
void quick_sort_by_price(Product products[], int low, int high);
void save_price_sorted_file(Product products[], int product_count);

int partition_by_code(Product products[], int low, int high);
void quick_sort_by_code(Product products[], int low, int high);
void save_code_sorted_file(Product products[], int product_count);

void search_product(Product products[], int product_count);
void delete_product(Product products[], int *product_count);
void add_product(Product products[], int *product_count);

int main() {
    Product products[MAX_PRODUCTS];
    int product_count = 0;
    int choice;

    srand((unsigned int)time(NULL));

    // Try loading existing database; if not present, create mock data
    product_count = load_from_file(products);
    if (product_count == 0) {
        printf("[INFO] Database file not found or empty. Initializing mock data...\n");
        initialize_mock_data(products, &product_count);
    } else {
        printf("[INFO] Loaded %d product(s) from 'products.txt'.\n", product_count);
    }

    do {
        printf("\n====================================\n");
        printf("   INVENTORY MANAGEMENT SYSTEM\n");
        printf("====================================\n");
        printf("1. Add New Product\n");
        printf("2. Delete Product\n");
        printf("3. Sort Products by Price (QuickSort)\n");
        printf("4. Sort Products by Code (QuickSort)\n");
        printf("5. Search Product\n");
        printf("0. Exit\n");
        printf("Select an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[ERROR] Invalid input! Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                add_product(products, &product_count);
                break;
            case 2:
                delete_product(products, &product_count);
                break;
            case 3:
                if (product_count > 0) {
                    quick_sort_by_price(products, 0, product_count - 1);
                    save_price_sorted_file(products, product_count);
                } else {
                    printf("\n[WARNING] Inventory is empty!\n");
                }
                break;
            case 4:
                if (product_count > 0) {
                    quick_sort_by_code(products, 0, product_count - 1);
                    save_code_sorted_file(products, product_count);
                } else {
                    printf("\n[WARNING] Inventory is empty!\n");
                }
                break;
            case 5:
                search_product(products, product_count);
                break;
            case 0:
                printf("\nExiting system. Goodbye!\n");
                break;
            default:
                printf("\n[ERROR] Invalid option. Please try again.\n");
                break;
        }
    } while (choice != 0);

    return 0;
}

int load_from_file(Product products[]) {
    FILE *file = fopen("products.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char header_code[50], header_price[50];
    // Skip header line if exists
    if (fscanf(file, "%s %s", header_code, header_price) != 2) {
        fclose(file);
        return 0;
    }

    int count = 0;
    while (count < MAX_PRODUCTS && fscanf(file, "%8s %d", products[count].code, &products[count].price) == 2) {
        count++;
    }

    fclose(file);
    return count;
}

void initialize_mock_data(Product products[], int *product_count) {
    *product_count = 3;
    for (int i = 0; i < *product_count; i++) {
        for (int j = 0; j < 4; j++) {
            products[i].code[j] = 'A' + (rand() % 26);
        }
        for (int j = 4; j < 8; j++) {
            products[i].code[j] = '0' + (rand() % 10);
        }
        products[i].code[8] = '\0';
        products[i].price = (rand() % 451) + 50;
    }
    save_to_main_file(products, *product_count);
}

void save_to_main_file(Product products[], int product_count) {
    FILE *file = fopen("products.txt", "w");
    if (file == NULL) {
        printf("\n[ERROR] Could not update database file!\n");
        return;
    }
    fprintf(file, "CODE\t\tPRICE\n");
    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%s\t\t%d\n", products[i].code, products[i].price);
    }
    fclose(file);
}

void add_product(Product products[], int *product_count) {
    if (*product_count >= MAX_PRODUCTS) {
        printf("\n[WARNING] Inventory maximum capacity reached!\n");
        return;
    }

    char input_code[CODE_LENGTH];
    printf("\nEnter new product code (exactly 8 characters): ");
    if (scanf("%8s", input_code) != 1) {
        printf("\n[ERROR] Failed to read product code.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    if (strlen(input_code) != 8) {
        printf("\n[ERROR] Product code must be exactly 8 characters long!\n");
        return;
    }

    if (is_code_exists(products, *product_count, input_code)) {
        printf("\n[ERROR] Product code '%s' already exists in inventory!\n", input_code);
        return;
    }

    int input_price;
    printf("Enter product price: ");
    if (scanf("%d", &input_price) != 1 || input_price <= 0) {
        printf("\n[ERROR] Invalid price! Must be a positive integer.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    strcpy(products[*product_count].code, input_code);
    products[*product_count].price = input_price;
    (*product_count)++;

    save_to_main_file(products, *product_count);
    printf("\n[SUCCESS] Product '%s' added successfully with price $%d.\n", input_code, input_price);
}

void delete_product(Product products[], int *product_count) {
    if (*product_count == 0) {
        printf("\n[WARNING] Inventory is empty!\n");
        return;
    }

    char target[CODE_LENGTH];
    printf("\nEnter product code to delete: ");
    if (scanf("%8s", target) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    for (int i = 0; i < *product_count; i++) {
        if (strcmp(products[i].code, target) == 0) {
            for (int j = i; j < (*product_count) - 1; j++) {
                products[j] = products[j + 1];
            }
            (*product_count)--;
            save_to_main_file(products, *product_count);
            printf("\n[SUCCESS] Product '%s' removed successfully.\n", target);
            return;
        }
    }
    printf("\n[ERROR] Product code '%s' not found.\n", target);
}

void search_product(Product products[], int product_count) {
    if (product_count == 0) {
        printf("\n[WARNING] Inventory is empty!\n");
        return;
    }

    char target[CODE_LENGTH];
    printf("\nEnter product code to search: ");
    if (scanf("%8s", target) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    for (int i = 0; i < product_count; i++) {
        if (strcmp(products[i].code, target) == 0) {
            printf("\n[FOUND] Product Code: %s | Price: $%d\n", products[i].code, products[i].price);
            return;
        }
    }
    printf("\n[NOT FOUND] Product with code '%s' does not exist.\n", target);
}

void swap_products(Product *a, Product *b) {
    Product temp = *a;
    *a = *b;
    *b = temp;
}

int partition_by_price(Product products[], int low, int high) {
    int pivot = products[high].price;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (products[j].price <= pivot) {
            i++;
            swap_products(&products[i], &products[j]);
        }
    }
    swap_products(&products[i + 1], &products[high]);
    return (i + 1);
}

void quick_sort_by_price(Product products[], int low, int high) {
    if (low < high) {
        int pi = partition_by_price(products, low, high);
        quick_sort_by_price(products, low, pi - 1);
        quick_sort_by_price(products, pi + 1, high);
    }
}

void save_price_sorted_file(Product products[], int product_count) {
    FILE *file = fopen("sorted_by_price.txt", "w");
    if (file == NULL) {
        printf("\n[ERROR] Could not open file for writing.\n");
        return;
    }
    fprintf(file, "------- PRODUCTS SORTED BY PRICE -------\n");
    fprintf(file, "CODE\t\tPRICE\n");
    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%s\t\t$%d\n", products[i].code, products[i].price);
    }
    fclose(file);
    printf("\n[SUCCESS] Products sorted by price and saved to 'sorted_by_price.txt'.\n");
}

int partition_by_code(Product products[], int low, int high) {
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (strcmp(products[j].code, products[high].code) < 0) {
            i++;
            swap_products(&products[i], &products[j]);
        }
    }
    swap_products(&products[i + 1], &products[high]);
    return (i + 1);
}

void quick_sort_by_code(Product products[], int low, int high) {
    if (low < high) {
        int pi = partition_by_code(products, low, high);
        quick_sort_by_code(products, low, pi - 1);
        quick_sort_by_code(products, pi + 1, high);
    }
}

void save_code_sorted_file(Product products[], int product_count) {
    FILE *file = fopen("sorted_by_code.txt", "w");
    if (file == NULL) {
        printf("\n[ERROR] Could not open file for writing.\n");
        return;
    }
    fprintf(file, "------- PRODUCTS SORTED BY CODE -------\n");
    fprintf(file, "CODE\t\tPRICE\n");
    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%s\t\t$%d\n", products[i].code, products[i].price);
    }
    fclose(file);
    printf("\n[SUCCESS] Products sorted by code and saved to 'sorted_by_code.txt'.\n");
}

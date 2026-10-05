# C Inventory Management System with QuickSort

A robust, file-persisted C application for managing product inventory with input validation and QuickSort implementation.

## Features
- **Dynamic Inventory Management**: Add, delete, and search products safely.
- **Data Persistence**: Automatically loads and saves inventory data from/to `products.txt`.
- **Input Validation**: Prevents duplicate product codes, buffer overflows, and invalid data types.
- **QuickSort Algorithm**: Custom QuickSort implementation to sort products by price ($O(n \log n)$) and product code.

## How to Run

1. Compile the code using `gcc`:
   ```bash
   gcc main.c -o inventory_system
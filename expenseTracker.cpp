#include <windows.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>  // For console input/output

using namespace std;

const int MAX_EXPENSES = 100;
struct Expense {
    double amount;
    string category;
    string date;
    string description;
};

const char FILE_PATH[] = "C:\\Users\\Hp\\Documents\\expenses.txt"; // Path to your file
Expense expenses[MAX_EXPENSES];
int count = 0;

void saveToFile(Expense expenses[], int count) {
    ofstream file(FILE_PATH, ios::out);
    if (!file) {
        MessageBox(NULL, "Error: Could not open file for writing.", "Error", MB_ICONERROR);
        return;
    }

    for (int i = 0; i < count; ++i) {
        file << expenses[i].amount << "\n";
        file << expenses[i].category << "\n";
        file << expenses[i].date << "\n";
        file << expenses[i].description << "\n";
    }

    file.close();
    MessageBox(NULL, "Expenses saved successfully!", "Success", MB_OK);
}

void loadFromFile(Expense expenses[], int &count) {
    ifstream file(FILE_PATH, ios::in);
    if (!file) {
        MessageBox(NULL, "No previous data found. Starting fresh.", "Info", MB_ICONINFORMATION);
        return;
    }

    count = 0;
    while (file >> expenses[count].amount) {
        file.ignore(); // Ignore newline character
        getline(file, expenses[count].category);
        getline(file, expenses[count].date);
        getline(file, expenses[count].description);
        count++;
        if (count >= MAX_EXPENSES) break;
    }

    file.close();
}

// Function to open console and get input from the user
void openConsoleAndGetInput() {
    // Open the console window
    AllocConsole();
    freopen("CONOUT$", "w", stdout); // Redirect output to console

    // Declare variables to store input
    double amount;
    string category, date, description;

    // Get user input
    cout << "Enter the amount: ";
    cin >> amount;
    cin.ignore();  // To discard any extra newlines

    cout << "Enter the category (e.g., Food, Rent): ";
    getline(cin, category);

    cout << "Enter the date (e.g., 2025-01-20): ";
    getline(cin, date);

    cout << "Enter the description: ";
    getline(cin, description);

    // Store input in expenses array
    expenses[count].amount = amount;
    expenses[count].category = category;
    expenses[count].date = date;
    expenses[count].description = description;

    // Save to file and increment count
    count++;
    saveToFile(expenses, count);

    // Close the console window after input
    fclose(stdout);
    FreeConsole();
}

// Function to view all expenses in a message box
void viewExpenses() {
    if (count == 0) {
        MessageBox(NULL, "No expenses recorded yet.", "Info", MB_ICONINFORMATION);
        return;
    }

    stringstream ss;
    for (int i = 0; i < count; ++i) {
        ss << "Expense " << i + 1 << ":\n";
        ss << "Amount: " << expenses[i].amount << "\n";
        ss << "Category: " << expenses[i].category << "\n";
        ss << "Date: " << expenses[i].date << "\n";
        ss << "Description: " << expenses[i].description << "\n";
        ss << "-------------------------\n";
    }

    MessageBox(NULL, ss.str().c_str(), "Expenses", MB_OK);
}

// Function to calculate the total of all expenses
void calculateTotal() {
    double total = 0;
    for (int i = 0; i < count; ++i) {
        total += expenses[i].amount;
    }

    stringstream ss;
    ss << "Total Expenses: " << total;
    MessageBox(NULL, ss.str().c_str(), "Total", MB_OK);
}

// Window procedure to handle events (button clicks)
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            CreateWindow("BUTTON", "Add Expense", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                150, 50, 100, 30, hwnd, (HMENU) 1, GetModuleHandle(NULL), NULL);
            CreateWindow("BUTTON", "View Expenses", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                150, 100, 100, 30, hwnd, (HMENU) 2, GetModuleHandle(NULL), NULL);
            CreateWindow("BUTTON", "Calculate Total", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                150, 150, 100, 30, hwnd, (HMENU) 3, GetModuleHandle(NULL), NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wp) == 1) {  // Add Expense
                openConsoleAndGetInput();  // Open the console for input
            }
            else if (LOWORD(wp) == 2) {  // View Expenses
                viewExpenses();  // Show expenses in GUI
            }
            else if (LOWORD(wp) == 3) {  // Calculate Total
                calculateTotal();  // Calculate and show total expenses
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }

    return 0;
}

// Entry point for the application
int main() {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ExpenseTrackerClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window registration failed!", "Error", MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindow(wc.lpszClassName, "Expense Tracker", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window creation failed!", "Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Load existing expenses
    loadFromFile(expenses, count);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

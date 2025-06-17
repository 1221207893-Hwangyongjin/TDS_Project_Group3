#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream> // for stringstream

#include <chrono> // time
#include <conio.h>  // for _getch()

using namespace std;

// ANSI Color Constants
const string RESET = "\033[0m";
const string BLUE = "\033[38;2;85;170;255m";  
const string YELLOW = "\033[38;2;255;245;155m";  

const string HEADER       = YELLOW + "----------------------------------------------------" + RESET;
const string HEADER_TITLE = YELLOW + "              SWIMMING ATTIRE SYSTEM                " + RESET;

const int MAX_PASSWORD_LENGTH = 6;
const int MAX_USERS = 100;
const int MAX_STAFF = 100;
const int MAX_ADMIN = 100;

class PaymentMethod;
class User;
class Staff;
class Admin;
class ADTstack;
class ADTstackOrder;

int getMaxIdFromFile(const string& filename) {
    ifstream infile(filename);
    
    if (!infile.is_open()) {
        return 1000; // If file not exist, start at 1001
    }

    string line;
    int maxId = 1000;

    while (getline(infile, line)) {
    	
        if (!line.empty()) {
            stringstream ss(line);
            string idStr;
            if (getline(ss, idStr, ',')) {
                
				try {
                    int id = stoi(idStr);
                    if (id > maxId) {
                        maxId = id;
                    }
                } catch (const exception& error) {
                	cout << "Some Error: "<< error.what() << endl;
                }
                
            }
        }
        
    }

    return maxId;
}

// product
struct Product {
    int id;
    string name;
    string category;
    string size;  // size
    string color; // color
    int quantity; // only for the User Cart quantity
    double price;
};

struct OrderHeader {
    int orderId;
    int userId;
    string username;
    double totalPrice;
    double finalPrice;
    string paymentMethod;
    string timestamp;
};

struct OrderItem {
	int itemId;
    int orderId;
    int productId;
    string productName;
    string category;
    string size;
    string color;
    int quantity;
    double unitPrice;
};

struct OperationRecord {
    string type;    // operation:"add" or "remove"
    Product product; // object of product want to operation
};

struct CategoryStats {
    string name;
    int orderCount;
    int itemCount;
    double totalSales;
};


// ================================================================================================================================ Payment Method

// PaymentMethod
class PaymentMethod {
protected:
    double amount;
    double discount;

public:
    PaymentMethod(double amt, double disc) : amount(amt), discount(disc) {}

    virtual void pay() = 0;

	virtual string getMethodName() = 0;

    double getDiscountedAmount() const {
        return amount * (1 - discount);
    }

    virtual ~PaymentMethod() {}
};

// subclass OnlineBanking
class OnlineBanking : public PaymentMethod {
private:
    string bankName; // record the bank name, example: Maybank, CIMB

public:
    OnlineBanking(double amt, const string& bank)
        : PaymentMethod(amt, 0.2), bankName(bank) {}  // assume dicsount is 20%

    string getMethodName() override {
        return "Online Banking";
    }

    void pay() override {
        cout << "Online Banking Payment via " << bankName << endl;
        cout << "Original amount: " << amount << endl;
        cout << "Discount: 20%" << endl;
        cout << "Final amount: " << getDiscountedAmount() << endl;
    }
};


// subclass CreditCard
class CreditCard : public PaymentMethod {
public:
    CreditCard(double amt) : PaymentMethod(amt, 0.1) {} // discount 10%

	string getMethodName() override {
	    return "Credit Card";
	}

    void pay() override {
        cout << "Credit Card Payment" << endl;
        cout << "Original amount: " << amount << endl;
        cout << "Discount: 10%" << endl;
        cout << "Final amount: " << getDiscountedAmount() << endl;
    }
};

// subclass EWallet
class EWallet : public PaymentMethod {
public:
    EWallet(double amt) : PaymentMethod(amt, 0.05) {} // discount 5%

	string getMethodName() override {
	    return "E-Wallet";
	}

    void pay() override {
        cout << "E-Wallet Payment" << endl;
        cout << "Original amount: " << amount << endl;
        cout << "Discount: 5%" << endl;
        cout << "Final amount: " << getDiscountedAmount() << endl;
    }
};

class QR_Code : public PaymentMethod {
public:
    QR_Code(double amt) : PaymentMethod(amt, 0.05) {}  // discount 5%

    string getMethodName() override {
        return "QR Code";
    }

    void pay() override {
        cout << "QR Code Payment" << endl;
        cout << "Original amount: " << amount << endl;
        cout << "Discount: 0%" << endl;
        cout << "Final amount: " << getDiscountedAmount() << endl;
    }
};

class Cash : public PaymentMethod {
public:
    Cash(double amt) : PaymentMethod(amt, 0.05) {}  // discount 5%

    string getMethodName() override {
        return "Cash";
    }

    void pay() override {
        cout << "Cash Payment" << endl;
        cout << "Original amount: " << amount << endl;
        cout << "Discount: 0%" << endl;
        cout << "Final amount: " << getDiscountedAmount() << endl;
    }
};


// class Stack for product
class ADTstack {
private:
    Product stack[100];
    int topstack;

public:
	Product* getStack() {
        return stack;
    }
	
    ADTstack() {
        topstack = -1;
    }

    bool empty() {
        return topstack == -1;
    }

    bool full() {
        return topstack == 99;
    }

    void push(const Product &p) {
        if (!full()) {
            topstack++;
            stack[topstack] = p;
        } else {
            cout << "Stack is Full" << endl;
        }
    }

    Product pop() {
        if (!empty()) {
            Product p = stack[topstack];
            topstack--;
            return p;
        } else {
            cout << "Stack is Empty" << endl;
            return Product{}; // avoid undefined
        }
    }
    
    int getTopIndex() {
        return topstack;
    }

    void display() {
        if (empty()) {
            cout << "Stack is Empty" << endl;
            return;
        }
        
        cout << "Products in Stack (Top to Bottom):\n";
        for (int i = topstack; i >= 0; --i) {
            Product &p = stack[i];
            cout << fixed << setprecision(2)
              << p.id << ", "
              << p.name << ", "
              << p.category << ", "
              << p.size << ", "
              << p.color << ", "
              << p.price << "\n";
        }
    }

    void saveToFile(string filename) {
        ofstream outfile(filename);
        
        for (int i = 0; i <= topstack; ++i) {
            Product &p = stack[i];
            
            outfile << fixed << setprecision(2)
              << p.id << ","
              << p.name << ","
              << p.category << ","
              << p.size << ","
              << p.color << ","
              << p.price << "\n";
        }
        
        cout << "Stack saved to file.\n";
    }

    void loadFromFile(string filename) {
        ifstream infile(filename);
        
	    if (!infile.is_open()) {
	        cout << "Error: cannot open file '" << filename << "' for reading.\n";
	        return;
	    }        
        
        string line;
        
        while (getline(infile, line)) {
            if (full()) {
                cout << "Stack is full. Some records were not loaded.\n";
                break;
            }
            stringstream ss(line);
            Product p;
            string tmp;

			// convert to int
            getline(ss, tmp, ',');
            p.id = stoi(tmp);
            
            getline(ss, p.name, ',');
            getline(ss, p.category, ',');
            getline(ss, p.size, ',');
            getline(ss, p.color, ',');
			
            getline(ss, tmp, ',');  
			p.price = stod(tmp);

            push(p);
        }
        
    } 
	    
    // Edit arbitrary product name by matching oldName -> newName
	bool editName(const string& oldName, const string& newName) {
	    if (empty()) 
			return false;
	
	    ADTstack tmp;
	    bool found = false;
	
	    while (!empty()) {
	        Product p = pop();
	        if (!found && p.name == oldName) {
	            p.name = newName;
	            found = true;
	        }
	        tmp.push(p);
	    }
	
	    while (!tmp.empty()) {
	        push(tmp.pop());
	    }
	
	    if (!found)
	        cout << "Name not found.\n";
	
	    return found;
	} 

    bool removeByName(const string& targetName) {
        if (empty()) 
			return false;
			
        ADTstack tmp;
        bool removed = false;
        
        while (!empty()) {
            Product p = pop();
            if (!removed && p.name == targetName) {
                removed = true;
                continue;
            }
            tmp.push(p);
        }
        
        while (!tmp.empty()) {
            push(tmp.pop());
        }
        
        if (!removed) 
			cout << "Product name \"" << targetName << "\" not found.\n";
			
        return removed;
    }
	
    bool removeById(const int& targetId) {
        if (empty()) 
			return false;
			
        ADTstack tmp;
        bool removed = false;
        
        while (!empty()) {
            Product p = pop();
            if (!removed && p.id == targetId) {
                removed = true;
                continue;
            }
            tmp.push(p);
        }
        
        while (!tmp.empty()) {
            push(tmp.pop());
        }
        
        if (!removed) 
			cout << "Product name \"" << targetId << "\" not found.\n";
			
        return removed;
    }  	 
    
    bool editById(int targetId) {
        for (int i = 0; i <= topstack; i++) {
            if (stack[i].id == targetId) {
            	
                // Edit Name
                cout << "Current name: " << stack[i].name << endl;
                cout << "Enter new name (press Enter to keep current): ";
                string newName;
                getline(cin, newName);
                if (!newName.empty()) {
                    stack[i].name = newName;
                }
                
                // Edit Category
                cout << "Current category: " << stack[i].category << endl;
                cout << "Enter new category (press Enter to keep current): ";
                string newCategory;
                getline(cin, newCategory);
                if (!newCategory.empty()) {
                    stack[i].category = newCategory;
                }
                
                // Edit size
                cout << "Current size: " << stack[i].size << endl;
                cout << "Enter new size (press Enter to keep current): ";
                string newSize;
                getline(cin, newSize);
                if (!newSize.empty()) {
                    stack[i].size = newSize;
                }
                
                // Edit color
                cout << "Current color: " << stack[i].color << endl;
                cout << "Enter new color (press Enter to keep current): ";
                string newColor;
                getline(cin, newColor);
                if (!newColor.empty()) {
                    stack[i].color = newColor;
                }
                
                // Edit Price with error handling
                cout << "Current price: " << stack[i].price << endl;
                cout << "Enter new price (press Enter to keep current): ";
                string priceInput;
                getline(cin, priceInput);
                
                if (!priceInput.empty()) {
                    try {
                        double newPrice = stod(priceInput);
                        stack[i].price = newPrice;
                    } catch (...) {
                        cout << "Invalid price format. Keeping current value.\n";
                    }
                }
                
                cout << "Product updated successfully!\n";
                return true;
            
            }
        }
        cout << "Product with ID " << targetId << " not found!\n";
        return false;
    }
    
    // Bubble Sort by chosen field
	void sortByField(int field) {
	    int n = topstack + 1;
	    bool swapped;
	
	    for (int i = 0; i < n - 1; ++i) { 
	        swapped = false;
	
	        for (int j = 0; j < n - i - 1; ++j) {
	            bool needSwap = false;  
	
	            switch (field) {
	                case 1: // Asc by id
	                    needSwap = stack[j].id > stack[j+1].id;
	                    break;
	                case 2: // Desc by id
	                    needSwap = stack[j].id < stack[j+1].id;
	                    break;
	                case 3: // Asc by name
	                    needSwap = stack[j].name > stack[j+1].name;
	                    break;
	                case 4: // Desc by name
	                    needSwap = stack[j].name < stack[j+1].name;
	                    break;
	                case 5: // Asc by category
	                    needSwap = stack[j].category > stack[j+1].category;
	                    break;
	                case 6: // Desc by category
	                    needSwap = stack[j].category < stack[j+1].category;
	                    break;
	                case 7: // Asc by size
	                    needSwap = stack[j].size > stack[j+1].size;
	                    break;
	                case 8: // Desc by size
	                    needSwap = stack[j].size < stack[j+1].size;
	                    break;
	                case 9: // Asc by color
	                    needSwap = stack[j].color > stack[j+1].color;
	                    break;
	                case 10: // Desc by color
	                    needSwap = stack[j].color < stack[j+1].color;
	                    break;
	                case 11: // Asc by price
	                    needSwap = stack[j].price > stack[j+1].price;
	                    break;
	                case 12: // Desc by price
	                    needSwap = stack[j].price < stack[j+1].price;
	                    break;
	                default:
	                    needSwap = false;  
	                    break;
	            }
	
	            
	            if (needSwap) {
	                swap(stack[j], stack[j+1]);
	                swapped = true;
	            }
	        }
	
	        if (!swapped) break;
	    }
	}
    
	int binarySearchById(int targetId) {
		int low  = 0;
        int high = topstack;
		
	    while (low <= high) {
	    	
	        int mid = low + (high - low) / 2;
	
	        // Check if x is present at mid
	        if (stack[mid].id == targetId)
	            return mid;
	
	        // If x greater, ignore left half
	        if (stack[mid].id < targetId)
	            low = mid + 1;
	
	        // If x is smaller, ignore right half
	        else
	            high = mid - 1;
	    }
	
	    // If we reach here, then element was not present
	    return -1;
	}

    //friend double User::calculateCartTotal();
    friend User;
};


class OperationStack {
private:
    OperationRecord stack[100];
    int topstack;

public:
    OperationStack() {
        topstack = -1;
    }

    bool empty() {
        return topstack == -1;
    }

    bool full() {
        return topstack == 99;
    }

    void push(const OperationRecord &r) {
        if (!full()) {
            topstack++;
            stack[topstack] = r;
        } else {
            cout << "Undo Stack is Full" << endl;
        }
    }

    OperationRecord pop() {
        if (!empty()) {
            OperationRecord r = stack[topstack];
            topstack--;
            return r;
        } else {
            cout << "Undo Stack is Empty" << endl;
            return OperationRecord{};
        }
    }

    OperationRecord* getStack() {
        return stack;
    }

    int getTopIndex() {
        return topstack;
    }
};



class User {
private:
	int userId;
    string username;
    string password;

	ADTstack cart; // User's Shopping Cart
	
    // Get the current time string
    string getCurrentTime() {
	    auto now = chrono::system_clock::now();
	    time_t now_time = chrono::system_clock::to_time_t(now);
	    tm now_tm = *localtime(&now_time);
	    
	    stringstream ss;
	    ss << put_time(&now_tm, "%Y-%m-%d");
	    return ss.str();
    }    
public:
   
    User(int inputUserId = 0 , string inputUsername = "", string inputPassword = "")
        :userId(inputUserId), username(inputUsername), password(inputPassword) {}

	// Getter username
    int getUserId() { 
		return userId; 
	}

    // Getter username
    string getUsername() { 
		return username; 
	}
	
	// Getter password
    string getPassword() { 
		return password;
	}

    // Setters
    void setUsername(string name) { username = name; }
    void setPassword(string pass) { password = pass; }

    static User* authenticate(const string& inputUsername, const string& inputPassword, const string& filename = "users.txt") {
    	
        ifstream userFile(filename);
        if (!userFile.is_open()) {
            cerr << "Error: Unable to open user file.\n";
            return nullptr;
        }

        string line;
        while (getline(userFile, line)) {
        	stringstream ss(line);
        	string idStr, parsedUsername, parsedPassword;
        	
        	getline(ss, idStr, ',');
            getline(ss, parsedUsername, ',');
            getline(ss, parsedPassword, ',');

            if (parsedUsername == inputUsername && parsedPassword == inputPassword) {
            	int parsedId = stoi(idStr);
                return new User(parsedId, parsedUsername, parsedPassword);
            }
        }

        return nullptr; // login failed
    }
    
    // Add to Cart
    void addToCart(const Product& product) {
        cart.push(product);
    }    
    
    // Save cart to file
    void saveCartToFile() {
        string filename = "user_" + to_string(userId) + ".txt";
        cart.saveToFile(filename);
    }    

	// Load cart from file    
    void loadCartFromFile() {
        string filename = "user_" + to_string(userId) + ".txt";
        cart.loadFromFile(filename);
    }   
	
	// display cart
    void displayCart() {
        cout << "===== Shopping Cart (" << username << ") =====\n";
        cart.display();
    }	 
	
	// save to orders.txt
    void saveOrderHeader(const OrderHeader& header) {
        ofstream file("orders.txt", ios::app);
        if (file.is_open()) {
            file << header.orderId << ","
                 << header.userId << ","
                 << header.username << ","
                 << header.paymentMethod << ","
                 << fixed << setprecision(2) << header.totalPrice << ","
                 << header.finalPrice << ","
                 << header.timestamp << "\n";
            file.close();
        } else {
            cerr << "Failed to save order header.\n";
        }
    }
    
    // svae to orderItems.txt
    void saveOrderItem(const OrderItem& item) {
        ofstream file("order_items.txt", ios::app);
        if (file.is_open()) {
            file << item.orderId << ","
                 << item.productId << ","
                 << item.productName << ","
                 << item.category << ","
                 << item.size << ","
                 << item.color << ","
                 << item.quantity << ","
                 << fixed << setprecision(2) << item.unitPrice << "\n";
            file.close();
        } else {
            cerr << "Failed to save order item.\n";
        }
    }
    
    // checkout: move cart to order history
    void checkout(PaymentMethod* payment) {
        // ??????ID
        int orderId = 1 + getMaxIdFromFile("orders.txt");
        
        // ????????
        double totalPrice = calculateCartTotal();
        
        // ???????
        double finalPrice = payment->getDiscountedAmount();
        
        // ????????
        OrderHeader header;
        header.orderId = orderId;
        header.userId = userId;
        header.username = username;
        header.paymentMethod = payment->getMethodName();
        header.totalPrice = totalPrice;
        header.finalPrice = finalPrice;
        header.timestamp = getCurrentTime();
        saveOrderHeader(header);
        
        // ???????????
        while (!cart.empty()) {
            Product product = cart.pop();
            
            // ?????????
            OrderItem item;
            item.orderId = orderId;
            item.productId = product.id;
            item.productName = product.name;
            item.category = product.category;
            item.size = product.size;
            item.color = product.color;
            item.quantity = 1; // ?????????1
            item.unitPrice = product.price;
            
            saveOrderItem(item);
        }
        
        cout << "Checkout completed! Order ID: " << orderId << "\n";
        cout << "Total: $" << totalPrice << ", After discount: $" << finalPrice << "\n";
    }
	
    // display order history
    void displayOrderHistory() {
        cout << "===== Order History (" << username << ") =====\n";
        
        // ????????
        ifstream headerFile("orders.txt");
        if (!headerFile.is_open()) {
            cout << "No order history found.\n";
            return;
        }
        
        string line;
        bool found = false;
        
        while (getline(headerFile, line)) {
            OrderHeader header;
            stringstream ss(line);
            string temp;
            
            // ?????
            getline(ss, temp, ',');
            header.orderId = stoi(temp);
            
            getline(ss, temp, ',');
            header.userId = stoi(temp);
            
            // ??????????
            if (header.userId != userId) continue;
            
            found = true;
            
            getline(ss, header.username, ',');
            getline(ss, header.paymentMethod, ',');
            
            getline(ss, temp, ',');
            header.totalPrice = stod(temp);
            
            getline(ss, temp, ',');
            header.finalPrice = stod(temp);
            
            getline(ss, header.timestamp);
            
            // ???????
            cout << "\n=== Order #" << header.orderId << " ===" << endl;
            cout << "Date: " << header.timestamp << endl;
            cout << "Payment: " << header.paymentMethod << endl;
            cout << "Total: $" << fixed << setprecision(2) << header.totalPrice;
            cout << ", Final: $" << header.finalPrice << endl;
            cout << "Items:\n";
            
            // ????????????
            ifstream itemFile("order_items.txt");
            if (itemFile.is_open()) {
                string itemLine;
                int itemCount = 0;
                
                while (getline(itemFile, itemLine)) {
                    OrderItem item;
                    stringstream itemSS(itemLine);
                    string itemTemp;
                    
                    getline(itemSS, itemTemp, ',');
                    int itemOrderId = stoi(itemTemp);
                    
                    // ??????????
                    if (itemOrderId != header.orderId) continue;
                    
                    getline(itemSS, itemTemp, ',');
                    item.productId = stoi(itemTemp);
                    
                    getline(itemSS, item.productName, ',');
                    getline(itemSS, item.category, ',');
                    getline(itemSS, item.size, ',');
                    getline(itemSS, item.color, ',');
                    
                    getline(itemSS, itemTemp, ',');
                    item.quantity = stoi(itemTemp);
                    
                    getline(itemSS, itemTemp, ',');
                    item.unitPrice = stod(itemTemp);
                    
                    // ??????
                    cout << "  - " << item.productName << " (" << item.category << ")";
                    cout << " Size: " << item.size << ", Color: " << item.color;
                    cout << ", Qty: " << item.quantity;
                    cout << ", Price: $" << item.unitPrice << endl;
                    
                    itemCount++;
                }
                
                if (itemCount == 0) {
                    cout << "  No items found for this order.\n";
                }
                
                itemFile.close();
            }
        }
        
        if (!found) {
            cout << "No orders found for this user.\n";
        }
        
        headerFile.close();
    }

    
    // get the grand total price from the shoppingcart
    double calculateCartTotal() {
        double total = 0.0;
        for (int i = cart.topstack; i >= 0; --i) {
        	// total += cart.stack[i].price * cart.stack[i].quantity;
            total += cart.stack[i].price * 1; // !!!------------ can improve to combine quantity
        }
        return total;
    }
    
    
    // Display user info
    void display() {
        cout << "ID: " << userId << ", Name: " << username << ", Password: " << password << endl;
    }

    // Static functions for file operations and admin management
    static void loadUsers(User users[], int &count) {
        ifstream file("users.txt");
        count = 0;
        string line;
        
	    while (getline(file, line) && count < MAX_USERS) {
	        stringstream ss(line);
	        string idStr, name, pass;
	        
	        if (getline(ss, idStr, ',') && getline(ss, name, ',') && getline(ss, pass)) {
	            
	            try {
	                int id = stoi(idStr);
	                users[count++] = User(id, name, pass);
	            } catch (invalid_argument e) {
	                cerr << "Invalid user ID format in line: " << line << endl;
	        	}
	        } else {
	        	
	            cerr << "Invalid user data format in line: " << line << endl;
	        }
	    }
        file.close();
    }

    static void saveUsers(User users[], int count) {
        ofstream file("users.txt");
        for (int i = 0; i < count; ++i) {
            file << users[i].getUserId() << "," << users[i].getUsername() << "," << users[i].getPassword() << endl;
        }
        file.close();
    }

    static void viewUsers(User users[], int count) {
        cout << "\n=== User List ===\n";
        for (int i = 0; i < count; ++i) {
            users[i].display();
        }
    }

    static void addUser(User users[], int &count) {
        if (count >= MAX_USERS) {
            cout << "User list is full.\n";
            return;
        }
        User newUser;
        
        newUser.userId = 1 + getMaxIdFromFile("users.txt");

        cout << "User ID: "  << newUser.userId << endl;
        cout << "Enter username: ";
        cin >> newUser.username;
        cout << "Enter password: ";
        cin >> newUser.password;
        users[count++] = newUser;
        saveUsers(users, count);
        cout << "User added successfully.\n";
    }

    static void editUser(User users[], int count) {
        int id;
        cout << "Enter user ID to edit: ";
        cin >> id;
        for (int i = 0; i < count; ++i) {
            if (users[i].getUserId() == id) {
                string newName, newPass;
                
                cout << "Enter new name: ";
                cin >> newName;
                cout << "Enter new password: ";
                cin >> newPass;
                users[i].setUsername(newName);
                users[i].setPassword(newPass);
                saveUsers(users, count);
                cout << "User updated.\n";
                return;
            }
        }
        cout << "User ID not found.\n";
    }

    static void deleteUser(User users[], int &count) {
        int id;
        cout << "Enter user ID to delete: ";
        cin >> id;
        for (int i = 0; i < count; ++i) {
            if (users[i].getUserId() == id) {
                for (int j = i; j < count - 1; ++j) {
                    users[j] = users[j + 1];
                }
                count--;
                saveUsers(users, count);
                cout << "User deleted.\n";
                return;
            }
        }
        cout << "User ID not found.\n";
    }    
    
};


class ReportManager {
private:
    static const int MAX_ORDERS = 100;
    static const int MAX_ITEMS = 300;
    static const int MAX_CATEGORIES = 20;
    
    OrderHeader orders[MAX_ORDERS];
    OrderItem items[MAX_ITEMS];
    
    int orderCount;
    int itemCount;
    
    string categories[MAX_CATEGORIES];
    int categoryCount;
    
    
public:
    ReportManager() : orderCount(0), itemCount(0), categoryCount(0) {
		loadCategories();
	}

    void loadCategories() {
        ifstream file("category.txt");
        if (!file.is_open()) {
            cerr << "Warning: category.txt not found. Using empty categories.\n";
            return;
        }

        string line;
        categoryCount = 0;
        while (getline(file, line) && categoryCount < MAX_CATEGORIES) {
            if (!line.empty()) {
                categories[categoryCount++] = line;
            }
        }
        file.close();
        
        if (categoryCount == 0) {
            cerr << "Warning: No categories found in category.txt\n";
        }
    }
    
    // loading the orders data
    void loadData() {

        ifstream headerFile("orders.txt");
        if (!headerFile.is_open()) {
            cerr << "Failed to open orders.txt\n";
            return;
        }
        
        string line;
        orderCount = 0;
        while (getline(headerFile, line) && orderCount < MAX_ORDERS) {
            stringstream ss(line);
            string temp;
            
            getline(ss, temp, ',');
            orders[orderCount].orderId = stoi(temp);
            
            getline(ss, temp, ',');
            orders[orderCount].userId = stoi(temp);
            
            getline(ss, orders[orderCount].username, ',');
            getline(ss, orders[orderCount].paymentMethod, ',');
            
            getline(ss, temp, ',');
            orders[orderCount].totalPrice = stod(temp);
            
            getline(ss, temp, ',');
            orders[orderCount].finalPrice = stod(temp);
            
            getline(ss, orders[orderCount].timestamp);
            
            orderCount++;
        }
        headerFile.close();
        
        // loading order items data
        ifstream itemFile("order_items.txt");
        if (!itemFile.is_open()) {
            cerr << "Failed to open order_items.txt\n";
            return;
        }
        
        itemCount = 0;
        while (getline(itemFile, line) && itemCount < MAX_ITEMS) {
            stringstream ss(line);
            string temp;
            
            getline(ss, temp, ',');
            items[itemCount].orderId = stoi(temp);
            
            getline(ss, temp, ',');
            items[itemCount].productId = stoi(temp);
            
            getline(ss, items[itemCount].productName, ',');
            getline(ss, items[itemCount].category, ',');
            getline(ss, items[itemCount].size, ',');
            getline(ss, items[itemCount].color, ',');
            
            getline(ss, temp, ',');
            items[itemCount].quantity = stoi(temp);
            
            getline(ss, temp, ',');
            items[itemCount].unitPrice = stod(temp);
            
            itemCount++;
        }
        itemFile.close();
        
    }
    
    // ????????
    void displaySummaryReport() {
        double totalRevenue = 0.0;
        double totalOriginal = 0.0;
        int totalItems = 0;
        int totalOrders = orderCount;
        
        for (int i = 0; i < orderCount; i++) {
            totalRevenue += orders[i].finalPrice;
            totalOriginal += orders[i].totalPrice;
        }
        
        for (int i = 0; i < itemCount; i++) {
            totalItems += items[i].quantity;
        }
        
        double discountPercentage = (totalOriginal > 0) ? 
            ((totalOriginal - totalRevenue) / totalOriginal * 100) : 0;
        
        cout << "\n===== ORDER SUMMARY REPORT =====\n";
        cout << "Total Orders:     " << totalOrders << "\n";
        cout << "Total Items Sold: " << totalItems << "\n";
        cout << "Total Revenue:    $" << fixed << setprecision(2) << totalRevenue << "\n";
        cout << "Original Value:   $" << totalOriginal << "\n";
        cout << "Total Discount:   $" << (totalOriginal - totalRevenue) 
             << " (" << discountPercentage << "%)\n";
        cout << "================================\n\n";
    }
    
    // ????????
    void displayCategoryReport() {
        // ???????
        if (categoryCount == 0) {
            loadCategories();
        }
    
        // ???????
        int orderCounts[MAX_CATEGORIES] = {0};
        int itemQuantities[MAX_CATEGORIES] = {0};
        double revenues[MAX_CATEGORIES] = {0.0};
    
        // ??????
        for (int i = 0; i < itemCount; i++) {
            for (int j = 0; j < categoryCount; j++) {
                if (items[i].category == categories[j]) {
                    itemQuantities[j] += items[i].quantity;
                    revenues[j] += items[i].unitPrice * items[i].quantity;
                    
                    // ????????
                    bool isNewOrder = true;
                    for (int k = 0; k < i; k++) {
                        if (items[k].orderId == items[i].orderId) {
                            isNewOrder = false;
                            break;
                        }
                    }
                    if (isNewOrder) {
                        orderCounts[j]++;
                    }
                    break;
                }
            }
        }
    
        // ????
        cout << "\n===== CATEGORY SALES REPORT =====\n";
        cout << "Category          Orders  Items  Revenue\n";
        cout << "--------------------------------------\n";
        
        int totalOrders = 0;
        int totalItems = 0;
        double totalRevenue = 0.0;
    
        for (int i = 0; i < categoryCount; i++) {
            cout << left << setw(18) << categories[i]
                 << right << setw(6) << orderCounts[i]
                 << setw(8) << itemQuantities[i]
                 << setw(10) << fixed << setprecision(2) << revenues[i]
                 << "\n";
            
            totalOrders += orderCounts[i];
            totalItems += itemQuantities[i];
            totalRevenue += revenues[i];
        }
    
        cout << "--------------------------------------\n";
        cout << left << setw(18) << "Total"
             << right << setw(6) << totalOrders
             << setw(8) << totalItems
             << setw(10) << totalRevenue << "\n";
        cout << "======================================\n\n";
    }
    
	void displayPaymentReport() {
	    // ????5?????
	    const string paymentMethods[5] = {
	        "Online Banking",
	        "Credit Card",
	        "E-Wallet",
	        "QR Code",
	        "Cash"
	    };
	    
	    // ??????? [count, amount]
	    int counts[5] = {0};
	    double amounts[5] = {0.0};
	    
	    // ??????
	    for (int i = 0; i < orderCount; i++) {
	        const string& method = orders[i].paymentMethod;
	        
	        // Directly match payment methods
	        for (int j = 0; j < 5; j++) {
	            if (method == paymentMethods[j]) {
	                counts[j]++;
	                amounts[j] += orders[i].finalPrice;
	                break;
	            }
	        }
	    }
	    
	    int totalCount = 0;
	    double totalAmount = 0.0;
	    for (int i = 0; i < 5; i++) {
	        totalCount += counts[i];
	        totalAmount += amounts[i];
	    }
	    
	    cout << "\n===== PAYMENT METHOD REPORT =====\n";
	    cout << "Method           Orders  Revenue\n";
	    cout << "-------------------------------\n";
	    
	    for (int i = 0; i < 5; i++) {
	        if (counts[i] > 0) { // ???????????
	            cout << left << setw(16) << paymentMethods[i]
	                 << right << setw(7) << counts[i]
	                 << setw(10) << fixed << setprecision(2) << amounts[i]
	                 << "\n";
	        }
	    }
	    
	    cout << "-------------------------------\n";
	    cout << left << setw(16) << "Total"
	         << right << setw(7) << totalCount
	         << setw(10) << totalAmount << "\n";
	    cout << "===============================\n\n";
	}
    
    // Display detailed order report
    void displayDetailedReport() {
        cout << "\n===== DETAILED ORDER REPORT =====\n";
        
        for (int i = 0; i < orderCount; i++) {
            const OrderHeader& header = orders[i];
            
            cout << "\n[Order #" << header.orderId << "]\n";
            cout << "User:      " << header.username << " (ID: " << header.userId << ")\n";
            cout << "Date:      " << header.timestamp << "\n";
            cout << "Payment:   " << header.paymentMethod << "\n";
            cout << "Total:     $" << fixed << setprecision(2) << header.totalPrice << "\n";
            cout << "Final:     $" << header.finalPrice << "\n";
            cout << "Discount:  $" << (header.totalPrice - header.finalPrice) 
                 << " (" << fixed << setprecision(1) 
                 << ((header.totalPrice > 0) ? (header.totalPrice - header.finalPrice) / header.totalPrice * 100 : 0)
                 << "%)\n";
            
            cout << "Items:\n";
            int itemCount = 0;
            double orderItemsTotal = 0.0;
            
            for (int j = 0; j < this->itemCount; j++) {
                if (items[j].orderId == header.orderId) {
                    cout << "  - " << items[j].productName 
                         << " (" << items[j].category << ") "
                         << "Size: " << items[j].size
                         << ", Color: " << items[j].color
                         << ", Qty: " << items[j].quantity
                         << ", Price: $" << fixed << setprecision(2) << items[j].unitPrice
                         << ", Subtotal: $" << (items[j].unitPrice * items[j].quantity)
                         << "\n";
                    itemCount++;
                    orderItemsTotal += items[j].unitPrice * items[j].quantity;
                }
            }
                        
            if (itemCount == 0) {
                cout << "  No items found for this order.\n";
            }
            
            cout << "--------------------------------\n";
        }
        
        cout << "\nTotal Orders Displayed: " << orderCount << "\n";
        cout << "===== END OF REPORT =====\n";
    }
    
    // Display all reports
    void displayFullReport() {
        loadData();
        displaySummaryReport();
        displayCategoryReport();
        displayPaymentReport();
        displayDetailedReport();
    }
    
};


class Staff {
private:
    int staffId;
    string name;
    string password;

public:
    // Constructor
    Staff(int id = 0, string n = "", string p = "") : staffId(id), name(n), password(p) {}

    // Getter
    int getStaffId() { 
		return staffId; 
	}
	
    string getName() { 
		return name; 
	}
    
	string getPassword() { 
		return password; 
	}
		

    // Setter
    void setName(string newName) { name = newName; }
    void setPassword(string newPass) { password = newPass; }

    static Staff* authenticate(const string& inputStaffname, const string& inputPassword, const string& filename = "staffs.txt") {
    	
        ifstream staffFile(filename);
        if (!staffFile.is_open()) {
            cerr << "Error: Unable to open user file.\n";
            return nullptr;
        }

        string line;
        while (getline(staffFile, line)) {
        	stringstream ss(line);
        	string idStr, parsedStaffname, parsedPassword;
        	
        	getline(ss, idStr, ',');
            getline(ss, parsedStaffname, ',');
            getline(ss, parsedPassword, ',');

            if (parsedStaffname == inputStaffname && parsedPassword == inputPassword) {
            	int parsedId = stoi(idStr);
                return new Staff(parsedId, parsedStaffname, parsedPassword);
            }
        }

        return nullptr; // login failed
    }

    // Static
    static void loadStaffs(Staff staffs[], int &count) {
	    count = 0;
	    ifstream file("staffs.txt");
	    if (!file) return;
	
		string line;
		
	    while (getline(file, line) && count < MAX_STAFF) {
	        stringstream ss(line);
	        string idStr, name, pass;
	        
	        if (getline(ss, idStr, ',') && getline(ss, name, ',') && getline(ss, pass)) {
	            
	            try {
	                int id = stoi(idStr);
	                staffs[count++] = Staff(id, name, pass);
	            } catch (...) {
	                cerr << "Invalid staff data: " << line << endl;
	            }
	        }
	    }
	
	    file.close();    	
    	
	}
	
    static void saveStaffs(Staff staffs[], int count){ 
	
	    ofstream file("staffs.txt");
	    for (int i = 0; i < count; ++i) {
	        file << staffs[i].getStaffId() << ","
	             << staffs[i].getName() << ","
	             << staffs[i].getPassword() << endl;
	    }
	    file.close();    	
	}
	
    static void viewStaffs(Staff staffs[], int count) {
	    cout << "All Staffs:\n";
	    for (int i = 0; i < count; ++i) {
	        cout << "ID: " << staffs[i].getStaffId()
	             << ", Name: " << staffs[i].getName() << endl;
	    }    	
	}
	
    static void addStaff(Staff staffs[], int &count) {
	    if (count >= MAX_STAFF) {
	        cout << "Staff limit reached.\n";
	        return;
	    }
	
	    int id = 1 + getMaxIdFromFile("users.txt");
        cout << "Auto-generated Staff ID: "  << id << endl;	   
		
		string name, pass; 
	    cout << "Enter name: ";
	    cin >> name;
	    cout << "Enter password: ";
	    cin >> pass;
	
	    staffs[count] = Staff(id, name, pass);
	    count++;
	    saveStaffs(staffs, count);
	    cout << "Staff added.\n"; 
    	
	}
	
    static void editStaff(Staff staffs[], int count) {
	    int id;
	    cout << "Enter staff ID to edit: ";
	    cin >> id;
	    for (int i = 0; i < count; ++i) {
	        if (staffs[i].getStaffId() == id) {
	            string newName, newPass;
	            cout << "Enter new name: ";
	            cin >> newName;
	            cout << "Enter new password: ";
	            cin >> newPass;
	            staffs[i].setName(newName);
	            staffs[i].setPassword(newPass);
	            saveStaffs(staffs, count);
	            cout << "Staff updated.\n";
	            return;
	        }
	    }
	    cout << "Staff ID not found.\n";    	
	}
    static void deleteStaff(Staff staffs[], int &count) {
	    int id;
	    cout << "Enter staff ID to delete: ";
	    cin >> id;
	    for (int i = 0; i < count; ++i) {
	        if (staffs[i].getStaffId() == id) {
	            for (int j = i; j < count - 1; ++j) {
	                staffs[j] = staffs[j + 1];
	            }
	            count--;
	            saveStaffs(staffs, count);
	            cout << "Staff deleted.\n";
	            return;
	        }
	    }
	    cout << "Staff ID not found.\n";    	
	}
};


class Admin {
private:
    int adminId;
    string name;
    string password;

public:
    // Constructor
    Admin(int id = 0, string n = "", string p = "") : adminId(id), name(n), password(p) {}

    // Getter
    int getAdminId() { 
        return adminId; 
    }
    
    string getName() { 
        return name; 
    }
    
    string getPassword() { 
        return password; 
    }
        

    // Setter
    void setName(string newName) { name = newName; }
    void setPassword(string newPass) { password = newPass; }

    static Admin* authenticate(const string& inputAdminname, const string& inputPassword, const string& filename = "admins.txt") {
        
        ifstream adminFile(filename);
        if (!adminFile.is_open()) {
            cerr << "Error: Unable to open user file.\n";
            return nullptr;
        }

        string line;
        while (getline(adminFile, line)) {
            stringstream ss(line);
            string idStr, parsedAdminname, parsedPassword;
            
            getline(ss, idStr, ',');
            getline(ss, parsedAdminname, ',');
            getline(ss, parsedPassword, ',');

            if (parsedAdminname == inputAdminname && parsedPassword == inputPassword) {
                int parsedId = stoi(idStr);
                return new Admin(parsedId, parsedAdminname, parsedPassword);
            }
        }

        return nullptr; // login failed
    }

    // Static
    static void loadAdmins(Admin admins[], int &count) {
        count = 0;
        ifstream file("admins.txt");
        if (!file) return;
    
        string line;
        
        while (getline(file, line) && count < MAX_ADMIN) {
            stringstream ss(line);
            string idStr, name, pass;
            
            if (getline(ss, idStr, ',') && getline(ss, name, ',') && getline(ss, pass)) {
                
                try {
                    int id = stoi(idStr);
                    admins[count++] = Admin(id, name, pass);
                } catch (...) {
                    cerr << "Invalid admin data: " << line << endl;
                }
            }
        }
    
        file.close();        
        
    }
    
    static void saveAdmins(Admin admins[], int count){ 
    
        ofstream file("admins.txt");
        for (int i = 0; i < count; ++i) {
            file << admins[i].getAdminId() << ","
                 << admins[i].getName() << ","
                 << admins[i].getPassword() << endl;
        }
        file.close();        
    }
    
    static void viewAdmins(Admin admins[], int count) {
        cout << "All Admins:\n";
        for (int i = 0; i < count; ++i) {
            cout << "ID: " << admins[i].getAdminId()
                 << ", Name: " << admins[i].getName() << endl;
        }        
    }
    
    static void addAdmin(Admin admins[], int &count) {
        if (count >= MAX_ADMIN) {
            cout << "Admin limit reached.\n";
            return;
        }
    
        int id = 1 + getMaxIdFromFile("admins.txt");
        cout << "Auto-generated Admin ID: " << id << endl;
        
		string name, pass;
        cout << "Enter name: ";
        cin >> name;
        cout << "Enter password: ";
        cin >> pass;
    
        admins[count] = Admin(id, name, pass);
        count++;
        saveAdmins(admins, count);
        cout << "Admin added.\n"; 
        
    }
    
    static void editAdmin(Admin admins[], int count) {
        int id;
        cout << "Enter admin ID to edit: ";
        cin >> id;
        for (int i = 0; i < count; ++i) {
            if (admins[i].getAdminId() == id) {
                string newName, newPass;
                cout << "Enter new name: ";
                cin >> newName;
                cout << "Enter new password: ";
                cin >> newPass;
                admins[i].setName(newName);
                admins[i].setPassword(newPass);
                saveAdmins(admins, count);
                cout << "Admin updated.\n";
                return;
            }
        }
        cout << "Admin ID not found.\n";        
    }
    
    static void deleteAdmin(Admin admins[], int &count) {
        int id;
        cout << "Enter admin ID to delete: ";
        cin >> id;
        for (int i = 0; i < count; ++i) {
            if (admins[i].getAdminId() == id) {
                for (int j = i; j < count - 1; ++j) {
                    admins[j] = admins[j + 1];
                }
                count--;
                saveAdmins(admins, count);
                cout << "Admin deleted.\n";
                return;
            }
        }
        cout << "Admin ID not found.\n";        
    }
};

// =============================================================================================
bool isUsernameTaken(const string& username) {
    ifstream file("users.txt");
    
    // check the file is open or not
    if (!file.is_open()) {
        cerr << "Error: Cannot open users.txt (file may not exist)" << endl;
        return false; // ?????????????
    }
    
    string line;
    
    while (getline(file, line)) {
        istringstream iss(line);
        string id, existingUsername;
        
        // read id and name
        getline(iss, id, ',');
        getline(iss, existingUsername, ','); // read until ,
        
        // compare username
        if (existingUsername == username) {
            file.close();
            return true;
        }
    }
    
    file.close();
    return false;
}

// Function to handle password input
string hidePassword(const string& prompt) {
    string password; // Store the password
    char ch;         // Store individual characters entered by the user

    while (true) {
        password.clear(); // Clear any previous password
        cout << prompt;

        while (true) {
            ch = _getch(); // Read a character without echoing to the screen
            if (ch == '\r') { // If Enter key is pressed, finish input
                if (password.empty()) {
                    cout << "\nPassword cannot be empty. Please try again.\n";
                    break; // Restart input
                } else if (password.length() < MAX_PASSWORD_LENGTH) {
                    cout << "\nPassword must be at least 8 characters long. Please try again.\n";
                    break; // Restart input
                }
                cout << endl; // Move to the next line
                return password; // Return the entered password
            }
            
            if (ch == '\b' && !password.empty()) { // Handle Backspace key
                cout << "\b \b"; // Erase the last asterisk from the screen
                password.pop_back(); // Remove the last character from the password
            } else if (password.length() < MAX_PASSWORD_LENGTH) { // Enforce maximum password length
                password += ch; // Add the character to the password
                cout << '*'; // Display an asterisk
            }
        }
    }
}

// =============================================================================================


// =============================================================================================

void basicAdminMenu(ADTstack &mainStack) {
    OperationStack undoStack;

    int choice;
    int maxId = getMaxIdFromFile("products.txt");
    
    do {
        cout << "\n=== Basic Operations (Undo Supported) ===\n"
             << "1. Display Products\n"
             << "2. Add Product\n"
             << "3. Delete Latest Product\n"
             << "4. Undo Operation\n"
             << "0. Return to Admin Menu & Save to File\n"
             << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        	case 1: {
        		mainStack.display();
				break;
			}
	        case 2: {  // Add Product
	            Product p;
	            
				p.id = ++maxId;
	            cout << "Product ID: " << p.id;
				           
	            cout << "Enter name: ";
				getline(cin, p.name);
	            
				cout << "Enter category: ";
				getline(cin, p.category);
	            
				cout << "Enter size: ";
				getline(cin, p.size);
				
				cout << "Enter color: ";
				getline(cin, p.color);
	            
	            // Add Price with error handling
	            bool validPrice = false;	            
	            while (!validPrice) {
	            	string priceInput;
					cout << "Enter price: ";
					getline(cin, priceInput);
					
					if (!priceInput.empty()) {
	                    try {
	                        p.price = stod(priceInput);
	                        validPrice = true;
	                    } catch (...) {
	                        cout << "Invalid price format. Please enter a valid number.\n";
	                    }
	                } else {
	                    cout << "Price is required. Please enter a value.\n";
	                }						
					
				}
	            mainStack.push(p);
	
	            OperationRecord record = {"remove", p};
	            undoStack.push(record);
				
	            cout << "Product added! (Undo available)\n";
	            break;
	        }
	        case 3: {  // Delete Latest Product
	            if (!mainStack.empty()) {
	                Product p = mainStack.pop();
	
	                OperationRecord record = {"add", p};
	                undoStack.push(record);
	
	                cout << "Deleted: "
	                     << p.id << ", "
	                     << p.name << ", "
	                     << p.category << ", "
	                     << p.size << ", "
	                     << p.color << ", "
	                     << p.price << "\n";
	            } else {
	                cout << "Stack is empty!\n";
	            }
	            break;
	        }
	        case 4: {  // Undo Operation
	            if (!undoStack.empty()) {
	                OperationRecord record = undoStack.pop();
	
	                if (record.type == "add") {
	                    mainStack.push(record.product);
	                    cout << "Undo completed: Restored product " << record.product.name << endl;
	                } 
	                else if (record.type == "remove") {
	                    if (!mainStack.empty() && mainStack.getStack()[mainStack.getTopIndex()].id == record.product.id) {
	                        mainStack.pop();
	                        cout << "Undo completed: Removed product " << record.product.name << endl;
	                    } else {
	                        cout << "Undo failed: Product state mismatch\n";
	                        undoStack.push(record);
	                    }
	                }
	                
	            } else {
	                cout << "No operations to undo!\n";
	            }
	            break;
	        }
	        			
			case 0: {
				cout << "Exiting...\n";
				mainStack.saveToFile("products.txt");
				break;
			}
        }
    } while (choice != 0);
}


void advanceAdminMenu(ADTstack &st) { // st == mainStack

	string filename = "products.txt";
    int choice;
    
    do {
        cout << "Menu:" << endl
             << "1. Display Products" << endl
             << "2. Edit Product" << endl
             << "3. Delete Product (by ID)" << endl
             << "4. Delete Product (by Name)" << endl
             << "0. Return to Admin Menu & Save to File" << endl
             << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
	        case 1:
	            st.display();
	            break;
			case 2: {
				
	            int id;
	            cout << "Enter product ID to edit: ";
	            cin >> id;
	            cin.ignore();
	            st.editById(id);
						
				break;
			}
			
			case 3: {
				int target;
				cout << "Enter product ID to delete: ";
				cin >> target;
				st.removeById(target);
				break;
			}


		    case 4: {
		        string target;
		        cout << "Enter product name to remove: ";
		        getline(cin, target);
		        st.removeByName(target);
		        break;
		    }
					           
	        case 0:
	            cout << "Exiting...\n";
	            st.saveToFile(filename);
	            break;
	        default:
	            cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    
}

void staffMenu(Staff* currentStaff) {
	
	ADTstack st;
    string filename = "products.txt";
    st.loadFromFile(filename);

	
	int choice;
	
		 	
	do {
		cout << "==== Product Management System ====\n"
		     << "1. Basic Mode    (Support Undo Mode)\n"
		     << "2. Advanced Mode (No Support Undo Mode)\n"
		     << "3. Order Report\n"
		     << "0. Exit\n"
		     << "Select mode: ";	         
	    cin >> choice;
	    
	    switch (choice) {
	        case 1:
	            cout << "You selected Basic Mode.\n";
	            basicAdminMenu(st); // Call basic mode function
	            break;
	        case 2:
	            cout << "You selected Advanced Mode.\n";
	            advanceAdminMenu(st); // Call advance mode function
	            break;
	            
		    case 3: {
			    ReportManager report;
			    
			    cout << "Generating order report...\n";
			    report.displayFullReport();
		        break;
		    }

	            
	        case 0:
	            cout << "Exiting the system. Goodbye!\n";
	            break;
	        default:
	            cout << "Invalid selection. Please try again.\n";
	            break;
	    }	    
	} while (choice != 0);		


}

void userMenu(User* currentUser){
    system("cls");
    
    ADTstack productStack;
	productStack.loadFromFile("products.txt");
	
	
	cout << "Login successful! Welcome, " << currentUser->getUsername() << "!\n\n";

    int choice;
    do {
        cout << "=== User Page ===" << endl
             << "1. Browse Products (All)" << endl
             << "2. Browse Products By Sorting" << endl
             << "3. View Shopping Cart" << endl
             << "4. Display Order History" << endl
             << "5. Delete Account (no implemented yet)" << endl
             << "0. Logout & Save Cart" << endl
             << "\nEnter choice: ";
        cin >> choice;
        cin.ignore();

		system("cls");
        switch (choice) {
        case 1: {     
			int targetId;
			   	
            cout << "Browse Products";
            productStack.display();
            
            cout << "Enter product ID to purchase: ";
			cin >> targetId;
            
			int index = productStack.binarySearchById(targetId);
			
            if (index != -1) {
                Product* arr = productStack.getStack();
				Product& foundProduct = arr[index]; // *(arr + index)
					
                cout << "Product Found:\n"
                     << "ID: "       << foundProduct.id << "\n"
                     << "Name: "     << foundProduct.name << "\n"
                     << "Category: " << foundProduct.category << "\n"
                     << "Size: " << foundProduct.size << "\n"
                     << "Color: " << foundProduct.color << "\n"
                     << "Price: $"   << foundProduct.price << "\n";
                    
                // purchase logic
                char confirm;
                cout << "Add to cart? (y/n): ";
                cin >> confirm;
                cin.ignore();

                if (confirm == 'y' || confirm == 'Y') {
                    currentUser->addToCart(foundProduct);
                    cout << "Added to cart!\n";
                }
                    
            } else {
                cout << "Product not found.\n";
            }    
            
            break;
		}
    
        case 2: {
        	ADTstack productStackForSorting;
			productStackForSorting.loadFromFile("products.txt");
			
			cout << "Choose sorting option:\n"
     			 << " 1. Ascending by id           2. Descending by id\n"
     			 << " 3. Ascending by name         4. Descending by name\n"
     			 << " 5. Ascending by category     6. Descending by category\n"
     			 << " 7. Ascending by size         8. Descending by size\n"
     			 << " 9. Ascending by color       10. Descending by color\n"
     			 << "11. Ascending by price       12. Descending by price\n"
     			 << "Enter option number: ";

            int field;
            cin >> field;
            //cin.ignore();
            
            productStackForSorting.sortByField(field);

            string fieldNames[13] = {
			    "", 
			    "ascending by id", "descending by id",
			    "ascending by name", "descending by name",
			    "ascending by category", "descending by category",
			    "ascending by size", "descending by size",
			    "ascending by color", "descending by color",
			    "ascending by price", "descending by price"
			}; // 1 = ascending by id, 2 = descending by id, .... 
                        
            
            if (field >= 1 && field <= 12)
                cout << "Products sorted by " << fieldNames[field] << ".\n";
            else
                cout << "Invalid field choice. No sorting applied.\n";
                
            productStackForSorting.display();
            
            break; 
		}
        case 3: {
            currentUser->displayCart();
            
		    string confirm;
		    cout << "Proceed to checkout? (y/n): ";
		    cin >> confirm;
		    cin.ignore();
		
		    if (confirm == "y" || confirm == "Y") {
		        double totalAmount = currentUser->calculateCartTotal();

		        cout << "\n=== Payment Methods ===" << endl;
		        cout << "1. Online Banking (20% discount)" << endl;
		        cout << "2. Credit Card (10% discount)" << endl;
		        cout << "Enter payment method (1-2): ";		        
		        
		        int paymentChoice;
		        cin >> paymentChoice;
		        cin.ignore();
		        
		        PaymentMethod* payment = nullptr;
		        
		        switch (paymentChoice) {
		            case 1: {
		                string bankName;
		                cout << "Enter bank name (e.g., Maybank, CIMB): ";
		                getline(cin, bankName);
		                payment = new OnlineBanking(totalAmount, bankName);
		                break;
		            }
		            case 2:
		                payment = new CreditCard(totalAmount);
		                break;
		            default:
		                cout << "Invalid payment method. Checkout cancelled." << endl;
		                return;
		        }		        
				
				// display payment details
		        cout << "\n=== Payment Summary ===" << endl;
		        cout << "Payment Method: " << payment->getMethodName() << endl;
		        payment->pay();
				
				// Confirmation of final payment
		        string finalConfirm;
		        cout << "\nConfirm payment? (y/n): ";
		        cin >> finalConfirm;
				        
		        if (finalConfirm == "y" || finalConfirm == "Y") {
		            currentUser->checkout(payment); // checkout and also save order history to file
		            cout << "Checkout completed. Cart cleared." << endl;
		           	delete payment;	
		        
				} else {		        	
		            cout << "Payment cancelled." << endl;
		            delete payment;	
		        }
		        
		    }
		    break;
		    
    	}
		
		case 4: {
			currentUser->displayOrderHistory();
			break;
		}
		
		case 5: {
            // First password verification
            string enteredPassword;
			system("cls");
			cout <<"Warning!" << endl;
			cout << "You are about to delete your user account." << endl;
			cout << "- All personal data associated with this account will be permanently deleted." << endl;
			cout << "- You will no longer have access to any services or content associated with this account." << endl;
			cout << "- Some transaction records may still be retained for compliance purposes, but will not include personal identifiers." << endl;
			cout << "- This action is IRREVERSIBLE, please ensure you have backed up all important data." << endl << endl;
			cout << "Please enter your password to verify that it is you and to agree to the above terms." << endl;
			cout << "Enter Password: ";
			
            cin >> enteredPassword;
                
            if (enteredPassword != currentUser->getPassword()) {
                cout << "Incorrect password! Account deletion cancelled.\n";
                break;
            }
                
            // Second confirmation
            int confirm;
			cout << "Are you sure want to delete current account? This operation cannot be undone." << endl;
			cout << "1. Yes" << endl;
			cout << "2. No" << endl;
			cout << "Choose (1 or 2): ";
            cin >> confirm;
                
            if (confirm != 1) {
                cout << "Account deletion cancelled.\n";
                break;
            }
 
            // Proceed with deletion
            ifstream inFile("users.txt");
            if (!inFile.is_open()) {
                cerr << "Error: Unable to open user file.\n";
                break;
            }
                
            ofstream outFile("temp_users.txt");
            if (!outFile.is_open()) {
                cerr << "Error: Unable to create temporary file.\n";
                inFile.close();
                break;
            }
                
            string line;
            bool found = false;
            bool success = true;
                
            while (getline(inFile, line)) {
                stringstream ss(line);
                string idStr, username, password;
                    
                getline(ss, idStr, ',');
                getline(ss, username, ',');
                getline(ss, password, ',');
                    
                if (username != currentUser->getUsername()) {
                    outFile << line << endl;
                } else {
                    found = true;
                }
            }
                
            inFile.close();
            outFile.close();
                
            if (found) {
                // Delete user's cart files if they exist
                string cartFile = "user_" + to_string(currentUser->getUserId()) + ".txt";
                    
                remove(cartFile.c_str()); // .c_str convert to char
                    
                // Replace original file
                if (remove("users.txt") != 0) { // 0 = successed , 0 != failed  
                    cerr << "Error deleting original user file.\n";
                    success = false;
                }
                else if (rename("temp_users.txt", "users.txt") != 0) {
                    cerr << "Error renaming temporary file.\n";
                    success = false;
                }
                    
                if (success) {
                    cout << "\nAccount deleted successfully. You will be logged out.\n";
                    system("pause");
                    choice = 0; // This will exit the menu loop
                } else {
                    cout << "Error occurred during account deletion.\n";
                }
            } else {
                remove("temp_users.txt");
                cout << "Error: Account not found in database.\n";
            }
						
			break;
		}
		
			
        case 0: {
        	currentUser->saveCartToFile();
            cout << "Cart saved. Logging out...\n";
            break;   
		}
 
        default:
            cout << "Invalid choice!";
        }
        
    } while (choice != 0);
    
}

void superAdminMenu(Admin* currentAdmin) {
	
    User users[MAX_USERS];
    int count = 0;

	Staff staffs[MAX_STAFF];
	int staffCount = 0 ;

	Admin admins[MAX_ADMIN];
	int adminCount = 0 ;

    int choice;

    User::loadUsers(users, count);
    Staff::loadStaffs(staffs, staffCount);
	Admin::loadAdmins(admins, adminCount);
	
    do {
		cout << "\n====================================";
		cout << "\n        SUPER ADMIN MENU            ";
		cout << "\n====================================";
		cout << "\n  User Management:";
		cout << "\n  1. View Users         2. Add User";
		cout << "\n  3. Edit User          4. Delete User";
		cout << "\n";
		cout << "\n  Staff Management:";
		cout << "\n  5. View Staff         6. Add Staff";
		cout << "\n  7. Edit Staff         8. Delete Staff";
		cout << "\n";
		cout << "\n  Admin Management:";
		cout << "\n  9. View Admins       10. Add Admin";
		cout << "\n 11. Edit Admin        12. Delete Admin";
		cout << "\n";
		cout << "\n  0. Back to Main Menu";
		cout << "\n====================================";
		cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: 
				User::viewUsers(users, count); 
				break;
            
			case 2: 
				User::addUser(users, count); 			
				break;
				
            case 3: 
				User::editUser(users, count); 
				break;
            
			case 4: 
				User::deleteUser(users, count); 
				break;
            
            case 5: 
                Staff::viewStaffs(staffs, staffCount); 
                break;
                
            case 6: 
                Staff::addStaff(staffs, staffCount); 
                break;
                
            case 7: 
                Staff::editStaff(staffs, staffCount); 
                break;
                
            case 8: 
                Staff::deleteStaff(staffs, staffCount); 
                break;
                
            case 9: 
                Admin::viewAdmins(admins, adminCount); 
                break;
                
            case 10: 
                Admin::addAdmin(admins, adminCount); 
                break;
                
            case 11: 
                Admin::editAdmin(admins, adminCount); 
                break;
                
            case 12: 
                Admin::deleteAdmin(admins, adminCount); 
                break;
				
			case 0: 
				break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);	
}

// Menu-driven program
int main() {

	int choice; 
	
	do {
	cout << HEADER << endl;
	cout << HEADER_TITLE << endl;
	cout << HEADER << endl;	
	cout << "1. User (Register)" << endl;
	cout << "2. User (Login)" << endl;
	cout << "3. Staff" << endl;
	cout << "4. Admin" << endl;
	cout << "0. Exit" << endl;
	cout << "Enter choice: ";	
	
    cin >> choice;
    cin.ignore();		
    
	system("cls");    
        switch (choice) {
        	case 1:{
        		
				cout << HEADER << endl;
				cout << YELLOW + "              USER REGISTER                " + RESET << endl;
				cout << HEADER << endl;
				    		
        		string username, password, confirmPassword;
        		
        		// Step 1 : Enter username (check if empty, if already exists)
        		do {
	            cout << "Enter username: ";
	            getline(cin, username);
	                
	                if (username.empty()) {
            		cout << "Username cannot be empty. Please try again." << endl;
           			continue;
					}
					
					if (isUsernameTaken(username)) {
            		cout << "Username already exists. Please choose another." << endl;
            		continue;
        			}

	            	break;
	            } while (true);
	            
			    // Step 2: enter password
				password = hidePassword("Enter password: ");	
				
			    // Step 3: confirm password			    
			    int attempts = 0;
			    bool registrationSuccess = false; // Track if the registration was successful
				
			    while (attempts < 3) {
			        confirmPassword = hidePassword("Confirm password : ");
			
			        if (password == confirmPassword) {
			            cout << "\nPassword confirmed successfully. Registration complete!" << endl;
			            registrationSuccess = true; // Mark registration as successful
						break;
			        } else {
			        	attempts++; // Increment the attempt counter
			            cout << "\nPasswords do not match. Attempts remaining: " << (3 - attempts) << endl;
			        }
			    }
				
				// Step 4: Register Fail (3 incorrect password attempts)
			    if (!registrationSuccess) {
			        cout << "Too many failed attempts. Registration aborted.\n";
			        system("pause"); // Pause before clearing screen
			        system("cls");
			        break; // Exit this case without proceeding to user registration
			    }
			    
			    // Step 5: Successful to register
				if (registrationSuccess) {
				    ofstream file("users.txt", ios::app);
				    
				    if (!file) {
				        cerr << "Error: Cannot save user data!" << endl;
				    } else {
				        int newID = 1 + getMaxIdFromFile("users.txt");
				        file << newID << "," << username << "," << password << "\n";
				        cout << "Registration saved!\n";
				    }
				    system("pause");
				    system("cls");
				}			    
			    
				break;													
			}
        	
	        case 2: {
				cout << HEADER << endl;
				cout << YELLOW + "              USER LOGIN                " + RESET << endl;
				cout << HEADER << endl;	        	
	        	
	            // Login first
	            string username, password;
	            
	            cout << "Please enter username: ";
				getline(cin, username);
				
	            cout << "Please enter password: ";
	            getline(cin, password);
	
	            User* currentUser = User::authenticate(username, password);
	            
	            if (currentUser) {
	                // if login success, go to the User Menu
	                userMenu(currentUser);		    
				    delete currentUser; 

	            } else {
	                cout << "Login failed: Incorrect username or password.\n";
	            }
	            
	            break;
	        }
	
	        case 3: {
				cout << HEADER << endl;
				cout << YELLOW + "              STAFF LOGIN                " + RESET << endl;
				cout << HEADER << endl;
				
	            // Login first
	            string username, password;
	            
	            cout << "Please enter Staff Name: ";
				getline(cin, username);
				
	            cout << "Please enter password: ";
	            getline(cin, password);
	
	            Staff* currentStaff = Staff::authenticate(username, password);
	            
	            if (currentStaff) {
	                // if login success, go to the User Menu
	                staffMenu(currentStaff);		    
				    delete currentStaff; 

	            } else {
	                cout << "Login failed: Incorrect username or password.\n";
	            }				
	            break;
	        } 
	        
	        case 4: {
	        	
	        	//superAdminMenu();
				cout << HEADER << endl;
				cout << YELLOW + "              ADMIN LOGIN                " + RESET << endl;
				cout << HEADER << endl;
				
	            // Login first
	            string adminname, password;
	            
	            cout << "Please enter Admin Name: ";
				getline(cin, adminname);
				
	            cout << "Please enter password: ";
	            getline(cin, password);
	
	            Admin* currentAdmin = Admin::authenticate(adminname, password);
	            
	            if (currentAdmin) {
	                // if login success, go to the User Menu
	                superAdminMenu(currentAdmin);		    
				    delete currentAdmin; 

	            } else {
	                cout << "Login failed: Incorrect username or password.\n";
	            }				
	        	
				break;
			}
	
	        case 0:{
	        	cout << "Exiting the program... Bye!\n";
				break;
			}
	            
	        default:
	            cout << "Invalid choice! Please try again\n";
        }        
        
	} while (choice != 0);

    return 0;
    
} // end of main

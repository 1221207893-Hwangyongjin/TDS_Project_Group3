#include <iostream>
#include <string>
#include <fstream>
#include <sstream> // for stringstream

#include <chrono>
#include <iomanip>

using namespace std;

class PaymentMethod;
class User;
class Admin;
class ADTstack;
class ADTstackOrder;

int getMaxCountLines(const string& filename) {
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

struct Order {
    int orderId; 
    int userId;
    string username;
    int productId;
    string productName;
    string category;
    string size;
    string color;
    int quantity;
    double totalPrice; 
    string paymentMethod;
    double finalAmount; // also call as grand total
    string timestamp;
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


class ADTstackOrder {
private:
    Order stack[100]; // sava order
    int topstack;

public:
    ADTstackOrder() : topstack(-1) {}

    bool empty() const {
        return topstack == -1;
    }

    bool full() const {
        return topstack == 99;
    }

    void push(const Order& o) {
        if (!full()) {
            stack[++topstack] = o;
        } else {
            cout << "Order stack is full!\n";
        }
    }

    Order pop() {
        if (!empty()) {
            return stack[topstack--];
        } else {
            cerr << "Order stack is empty!\n";
            return Order{};
        }
    }
    

    // load file from order
    void loadOrdersFromFile(const string& filename, const string& targetUsername) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No existing order history.\n";
            return;
        }

        string line;
        
        while (getline(file, line)) {
            if (full()) 
				break;

            Order order;
            string tmp;
            stringstream ss(line);
            
            getline(ss, tmp, ',');
            order.orderId = stoi(tmp);
            
        	getline(ss, tmp, ',');
        	order.userId = stoi(tmp);
		            
            getline(ss, order.username, ',');
            
			getline(ss, tmp, ',');
            order.productId = stoi(tmp);
            
            getline(ss, order.productName, ',');

            getline(ss, tmp, ','); 
			order.quantity = stoi(tmp);
			
            getline(ss, tmp, ','); 
			order.totalPrice = stod(tmp);
			
            getline(ss, order.timestamp);
			
			// only save the current user's orders
	        if (order.username == targetUsername) {
	            push(order);
	        }
        }
    }

    // display order history
    void displayOrders() const {
        if (empty()) {
            cout << "No orders found.\n";
            return;
        }
        
        cout << "===== Order History =====\n";
        for (int i = topstack; i >= 0; --i) {
            const Order& order = stack[i];
            cout << "Order ID: " << order.orderId    << "\n"
                 << "User:     " << order.username   << "\n"
                 << "Product:  " << order.productName << " (ID: " << order.productId << ")\n"
                 << "Categories: " << order.category  << "\n"
                 << "Size: " << order.size   << "\n"
                 << "Color: " << order.color   << "\n"
                 << "Total:    $" << order.totalPrice << "\n"
                 << "Time:     " << order.timestamp  << "\n\n";
        }
    }
};


class User {
private:
	int userId;
    string username;
    string password;

	ADTstack cart; // User's Shopping Cart
	ADTstackOrder orderHistory;
	
    // Get the current time string
    string getCurrentTime() {
	    auto now = chrono::system_clock::now();
	    time_t now_time = chrono::system_clock::to_time_t(now);
	    tm now_tm = *localtime(&now_time);
	    
	    stringstream ss;
	    ss << put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
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
    
    // checkout: move cart to order history
	void checkout(PaymentMethod* payment) {
		
		int orderIdForSave = 1 + getMaxCountLines("orders.txt");
		
	    while (!cart.empty()) {
	        Product product = cart.pop();
	        
	        // Generate order
	        Order order;
	        
	        order.orderId = orderIdForSave; // Generate a unique order ID
	        order.userId = userId;
	        order.username = username;
	        
	        order.productId = product.id;
	        order.productName = product.name;
	        order.quantity = 1; // Assume 1 item per purchase
	        
			order.totalPrice = product.price;
	        order.paymentMethod = payment->getMethodName();
	        order.finalAmount = payment->getDiscountedAmount();
	        order.timestamp = getCurrentTime(); // Get the current time
	        
	        orderHistory.push(order); // Store in order history
	        
	        ofstream file("orders.txt", ios::app);
	        if (file.is_open()) {
	            file << order.orderId << ","
	                 << order.userId << ","
	                 << order.username << ","
	                 
	                 << order.productId << ","
	                 << order.productName << ","
	                 << order.quantity << ","
	                 
	                 << order.totalPrice << ","
	                 << order.paymentMethod << ","
	                 << order.finalAmount << ","
	                 << order.timestamp << "\n";
	            file.close();
	        } else {
	            cerr << "Failed to save order to file.\n";
	        }
	        
	        orderIdForSave++; // continue plus orderIdForSave
				        
	    }
	}
	
    // display order history
    void displayOrderHistory() {
        cout << "===== Order History (" << username << ") =====\n";
        orderHistory.displayOrders();
    }

    // load order history from file 
	void loadOrderHistory() {
	    orderHistory.loadOrdersFromFile("orders.txt", username);
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
    
};


void adminMenu() {
	ADTstack st;
    const string filename = "products.txt";
    st.loadFromFile(filename);

    int choice;
    do {
        cout << "Menu:" << endl
             << "1. Display Products" << endl
             << "2. Add a Product" << endl
             << "3. Remove a Product (Top)" << endl
             << "4. Save to File" << endl
             << "5. Edit a Product" << endl
             << "6. Remove Product by Name" << endl
             << "0. Exit" << endl
             << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            st.display();
            break;
        case 2: {
            Product p;
            
			p.id = 1 + getMaxCountLines(filename);
            cout << "Product Id: "<< p.id << endl;      
			
            cout << "Enter name: ";
			getline(cin, p.name);
            
			cout << "Enter category: ";
			getline(cin, p.category);
            
			cout << "Enter size: ";
			getline(cin, p.size);
			
			cout << "Enter color: ";
			getline(cin, p.color);
            
			cout << "Enter price: ";
			cin >> p.price;
            
			st.push(p);
            break;
        }
        case 3: {
        	if (!st.empty()) {
            	Product p = st.pop();
                cout << "Removed: "
                     << p.id << ", "
                     << p.name << ", "
                     << p.category << ", "
                     << p.size << ", "
                     << p.color << ", "
                     << p.price << "\n";
       		}
            break;
        }
        case 4:
            st.saveToFile(filename);
            break;

        case 5: {
            string oldN, newN;
            cout << "Enter existing product name to edit: ";
            getline(cin, oldN);
            cout << "Enter new product name: ";
            getline(cin, newN);
            st.editName(oldN, newN);
            break;
        }
       
	    case 6: {
	        string target;
	        cout << "Enter product name to remove: ";
	        getline(cin, target);
	        st.removeByName(target);
	        break;
	    }       
       
        case 0:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    
}

void userMenu(User* currentUser){
    system("cls");
    
    ADTstack productStack;
	productStack.loadFromFile("products.txt");
	
	// Load Shopping Cart when user Login
	currentUser->loadOrderHistory();
	
	cout << "Login successful! Welcome, " << currentUser->getUsername() << "!\n\n";

    int choice;
    do {
        cout << "=== User Page ===" << endl
             << "1. Browse Products (All)" << endl
             << "2. Browse Products By Sorting" << endl
             << "3. View Shopping Cart" << endl
             << "4. Display Order History" << endl
             << "5. Delete Account (no implemented yet)"
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
		        } else {
		            delete payment;
		            cout << "Payment cancelled." << endl;
		        }	
		        
		    }
		    break;
		    
    	}
		
		case 4: {
			currentUser->displayOrderHistory();
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

// Menu-driven program
int main() {

	int choice; 
	
	do {
	cout << "Main Menu:" << endl;
	cout << "1. User (Login)" << endl;
	cout << "2. Admin" << endl;
	cout << "0. Exit" << endl;
	cout << "Enter choice: ";

        cin >> choice;
        cin.ignore();		
        
        switch (choice) {
	        case 1: {
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
	
	        case 2: {
				adminMenu();
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

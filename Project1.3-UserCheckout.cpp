#include <iostream>
#include <string>
#include <fstream>
#include <sstream> // for stringstream

#include <chrono>
#include <iomanip>

using namespace std;

int countLines(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return 0; // if file no exist
    }
    
    int count = 0;
    string line;
    while (getline(file, line)) {
        if (!line.empty()) { // Ignore spaces
            count++;
        }
    }
    return count;
}

// product
struct Product {
    string id;
    string name;
    string category;
    int quantity;
    double price;
};

struct Order {
    string orderId; 
    string username;
    string productId;
    string productName;
    int quantity;
    double totalPrice; 
    string timestamp;
};


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
            cout 
              << p.id       << ", "
              << p.name     << ", "
              << p.category << ", "
              << p.quantity << ", "
              << p.price    << "\n";
        }
    }

    void saveToFile(const string &filename) {
        ofstream file(filename);
        
        for (int i = 0; i <= topstack; ++i) {
            Product &p = stack[i];
            file 
              << p.id       << ","
              << p.name     << ","
              << p.category << ","
              << p.quantity << ","
              << p.price    << "\n";
        }
        
        cout << "Stack saved to file.\n";
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        
	    if (!file.is_open()) {
	        cout << "Error: cannot open file '" << filename << "' for reading.\n";
	        return;
	    }        
        
        string line;
        
        while (getline(file, line)) {
            if (full()) {
                cout << "Stack is full. Some records were not loaded.\n";
                break;
            }
            stringstream ss(line);
            Product p;
            string tmp;

            getline(ss, p.id,       ',');
            getline(ss, p.name,     ',');
            getline(ss, p.category, ',');

            // convert to int
            getline(ss, tmp, ',');  
			p.quantity = stoi(tmp);
			
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
	                case 1:
	                    needSwap = stack[j].id > stack[j+1].id;
	                    break;
	                case 2:
	                    needSwap = stack[j].name > stack[j+1].name;
	                    break;
	                case 3:
	                    needSwap = stack[j].category > stack[j+1].category;
	                    break;

	                case 7:
	                    needSwap = stack[j].quantity > stack[j+1].quantity;
	                    break;
	                case 8:
	                    needSwap = stack[j].price > stack[j+1].price;
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
    
	int binarySearchById(string targetId) {
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
            stringstream ss(line);
            getline(ss, order.orderId,    ',');
            getline(ss, order.username,   ',');
            getline(ss, order.productId,  ',');
            getline(ss, order.productName, ',');
            
            string tmp;
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
                 << "Quantity: " << order.quantity   << "\n"
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
   
    User(const string& uname = "", const string& pwd = "")
        : username(uname), password(pwd) {}

    // Getter username
    string getUsername() const { 
		return username; 
	}
	
	// Getter password
    string getPassword() const { 
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
        	string parsedUsername, parsedPassword;
        	
            getline(ss, parsedUsername, ',');
            getline(ss, parsedPassword, ',');

            if (parsedUsername == inputUsername && parsedPassword == inputPassword) {
                return new User(parsedUsername, parsedPassword);
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
        string filename = "user_" + username + ".txt";
        cart.saveToFile(filename);
    }    

	// Load cart from file    
    void loadCartFromFile() {
        string filename = "user_" + username + ".txt";
        cart.loadFromFile(filename);
    }   
	
	// display cart
    void displayCart() {
        cout << "===== Shopping Cart (" << username << ") =====\n";
        cart.display();
    }	 
    
    // checkout: move cart to order history
	void checkout() {
		
		int orderIdForSave = 1001 + countLines("orders.txt");
		
	    while (!cart.empty()) {
	        Product product = cart.pop();
	        
	        // Generate order
	        Order order;
	        order.orderId = to_string(orderIdForSave); // Generate a unique order ID
	        order.username = username;
	        order.productId = product.id;
	        order.productName = product.name;
	        order.quantity = 1; // Assume 1 item per purchase
	        order.totalPrice = product.price;
	        order.timestamp = getCurrentTime(); // Get the current time
	        
	        orderHistory.push(order); // Store in order history
	        
	        ofstream file("orders.txt", ios::app);
	        if (file.is_open()) {
	            file << order.orderId << ","
	                 << order.username << ","
	                 << order.productId << ","
	                 << order.productName << ","
	                 << order.quantity << ","
	                 << order.totalPrice << ","
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
            cout << "Enter id: ";        getline(cin, p.id);
            cout << "Enter name: ";      getline(cin, p.name);
            cout << "Enter category: ";  getline(cin, p.category);
            cout << "Enter quantity: ";  cin >> p.quantity; cin.ignore();
            cout << "Enter price: ";     cin >> p.price;    cin.ignore();
            st.push(p);
            break;
        }
        case 3: {
            Product p = st.pop();
            if (!p.id.empty())
                cout << "Removed: "
                     << p.id << ", "
                     << p.name << ", "
                     << p.category << ", "
                     << p.quantity << ", "
                     << p.price << "\n";
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
             << "0. Logout & Save Cart" << endl
             << "\nEnter choice: ";
        cin >> choice;
        cin.ignore();

		system("cls");
        switch (choice) {
        case 1: {        	
            cout << "Browse Products";
            productStack.display();
            
            cout << "Enter product ID to purchase: ";
			string targetId;
			getline(cin, targetId);
            
			int index = productStack.binarySearchById(targetId);
			
            if (index != -1) {
                Product* arr = productStack.getStack();
				Product& foundProduct = arr[index]; // *(arr + index)
					
                cout << "Product Found:\n"
                     << "ID: "       << foundProduct.id << "\n"
                     << "Name: "     << foundProduct.name << "\n"
                     << "Category: " << foundProduct.category << "\n"
                     << "Quantity: " << foundProduct.quantity << "\n"
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
			cout << "Choose field to sort by:\n"
            	 << "1. id\n"
                 << "2. name\n"
                 << "3. category\n"
                 << "4. size\n"
                 << "5. color\n"
                 << "6. material\n"
                 << "7. quantity\n"
                 << "8. price\n"
                 << "Enter field number: ";
            int field;
            cin >> field;
            //cin.ignore();
            
            productStack.sortByField(field);
            string fieldNames[9] = {"", "id", "name", "category", "size", "color", "material", "quantity", "price"}; // 1 = id, 2 = name, .... 
            
            if (field >= 1 && field <= 8)
                cout << "Products sorted by " << fieldNames[field] << ".\n";
            else
                cout << "Invalid field choice. No sorting applied.\n";
                
            productStack.display();
            
            break; 
		}
        case 3: {
            currentUser->displayCart();
            
		    char confirm;
		    cout << "Proceed to checkout? (y/n): ";
		    cin >> confirm;
		    cin.ignore();
		
		    if (confirm == 'y' || confirm == 'Y') {
		        currentUser->checkout(); // checkout and also save order history to file
		        cout << "Checkout completed. Cart cleared.\n";
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
        cout << "Menu:\n"
             << "1. User (Login)\n"
             << "2. Admin\n"
             << "0. Exit\n"
             << "Enter choice: ";
        cin >> choice;
        cin.ignore();		
        
        switch (choice) {
        case 1: {
            // Login first
            string username, password;
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
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

        case 0:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }        
        
	} while (choice != 0);

    return 0;
}
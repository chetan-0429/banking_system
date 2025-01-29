#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <random>
#include <set>
using namespace std;

class UserDatabase {

private:
    unordered_map<string, pair<string, double>> userRecords;
    unordered_map<string, string> adminAccounts;
public:
    UserDatabase() {
        loadUserData();
        loadAdminData();
    }

    void saveUserData() {
        ofstream userDataFile("user_data.txt");

        for (const auto &user : userRecords) {
            userDataFile << user.first << " " << user.second.first << " " << user.second.second << endl;
        }
        userDataFile.close();
    }

    bool loadUserData() {
        ifstream userDataFile("user_data.txt");
        if (!userDataFile) {
            return false;
        }

        string line;
        while (getline(userDataFile, line)) {
            istringstream stream(line);
            string username, password;
            double balance;
            if (stream >> username >> password >> balance) {
                userRecords[username] = make_pair(password, balance);
            }
        }

        userDataFile.close();
        return true;
    }

    bool loadAdminData() {
        ifstream adminFile("admin_data.txt");
        if (!adminFile) {
            return false;
        }

        string line;
        while (getline(adminFile, line)) {
            istringstream stream(line);
            string adminUsername, adminPassword;
            if (stream >> adminUsername >> adminPassword) {
                adminAccounts[adminUsername] = adminPassword;
            }
        }
        adminFile.close();
        return true;
    }

    void createNewUser(const string &username, const string &password) {
        userRecords[username] = make_pair(password, 0.0);
        saveUserData();
        cout << "Account successfully created!" << endl;
    }

    void removeUser(const string &username) {
        userRecords.erase(username);
    }

    bool authenticateUser(string &username, string &password) {
        cout << "Enter your username: ";
        cin >> username;

        if (userRecords.find(username) != userRecords.end()) {
            cout << "Enter your password: ";
            cin >> password;

            if (userRecords[username].first == password) {
                return true;
            }
        }
        cout << "Invalid credentials. Please try again." << endl;
        return false;
    }

    pair<string, double>& getUserData(const string &username) {
        return userRecords[username];
    }

};

class TransactionRecord {
public:
    string transactionType;
    double transactionAmount;

    TransactionRecord(const string &type, double amount) : transactionType(type), transactionAmount(amount) {}
};

class BankAccountManager {
private:
    string username;
    bool isActive;
    double balance;
    vector<TransactionRecord> transactionHistory;
    UserDatabase &db;

public:
    BankAccountManager(const string &accountUsername, UserDatabase &db) : username(accountUsername), isActive(true), balance(0), db(db) {}

    void loadUserData() {
        balance = db.getUserData(username).second;
    }

    void withdrawFunds(double amount) {
        if (isActive && amount <= balance && amount > 0) {
            balance -= amount;
            // transactionHistory.emplace_back("Withdrawal", amount);
        TransactionRecord transaction("Withdrawal", amount);
        transactionHistory.push_back(transaction);
        
            cout << "Successfully withdrawn $" << amount << ". New balance: " << balance << endl;
        } else {
            cout << "Invalid amount or account is inactive. Please contact support." << endl;
        }
    }

    void depositFunds(double amount) {
        if (isActive && amount > 0) {
            balance += amount;
            transactionHistory.emplace_back("Deposit", amount);
            cout << "Successfully deposited $" << amount << ". New balance: " << balance << endl;
        } else {
            cout << "Invalid amount or account is inactive. Please contact support." << endl;
        }
    }

    void displayAccountDetails() {
        cout << "Account Holder: " << username << endl;
        cout << "Current Balance: $" << balance << endl;
        cout << "Transaction History:" << endl;
        for (const TransactionRecord &transaction : transactionHistory) {
            cout << transaction.transactionType << ", Amount: $" << transaction.transactionAmount << endl;
        }
        cout << endl;
    }

    void saveAccountData() {
        db.getUserData(username).second = balance;
        db.saveUserData();
    }

    void closeAccount() {
        isActive = false;
        db.removeUser(username);
        db.saveUserData();
        cout << "Account has been successfully closed." << endl;
    }
};

class AdminAccess {
public:
    AdminAccess() {
        loadAdminData();
    }

    bool authenticateAdmin() {
        string username, password;
        cout << "Enter admin username: ";
        cin >> username;
        if (adminAccounts.find(username) != adminAccounts.end()) {
            cout << "Enter admin password: ";
            cin >> password;

            if (adminAccounts[username] == password) {
                return true;
            }
        }
        cout << "Admin login failed. Try again." << endl;
        return false;
    }

private:
    unordered_map<string, string> adminAccounts;

    void loadAdminData() {
        ifstream adminFile("admin_data.txt");
        if (!adminFile) {
            return;
        }

        string line;
        while (getline(adminFile, line)) {
            istringstream stream(line);
            string adminUsername, adminPassword;
            if (stream >> adminUsername >> adminPassword) {
                adminAccounts[adminUsername] = adminPassword;
            }
        }
        adminFile.close();
    }
};

int main() {
    UserDatabase userDB;
    AdminAccess admin;
    string username, password;
    double initialBalance;
    BankAccountManager* account = nullptr;  
    int userChoice;
    int optionChoice;
    double withdrawalAmount;
    double depositAmount;
    bool userLoggedIn = false;
    bool adminLoggedIn = false;

    while (true) {
        if (!userLoggedIn) {
            cout << "1. User Login\n2. Create New Account\n3. Admin Login\n4. Exit\n";
            cin >> userChoice;

            if (userChoice == 1) {
                if (userDB.authenticateUser(username, password)) {
                    userLoggedIn = true;
                    account = new BankAccountManager(username, userDB); 
                    account->loadUserData();
                }
            } else if (userChoice == 2) {
                string newUsername, newPassword;
                cout << "Enter your new username: ";
                cin >> newUsername;
                cout << "Enter your new password: ";
                cin >> newPassword;
                userDB.createNewUser(newUsername, newPassword);
                username = newUsername;
                account = new BankAccountManager(username, userDB);  
                account->loadUserData();
            } else if (userChoice == 3) {
                if (admin.authenticateAdmin()) {
                    userLoggedIn = true;
                    adminLoggedIn = true;
                    cout << "Admin Menu\n";
                    cout << "1. View All User Accounts\n2. Logout\n";
                    cin >> optionChoice;
                    if (optionChoice == 1) {
                        ifstream userFile("user_data.txt");
                        if (userFile.is_open()) {
                            string line;
                            while (getline(userFile, line)) {
                                cout << line << endl;
                            }
                            userFile.close();
                        }
                    }
                    if (optionChoice == 2) {
                        adminLoggedIn = false;
                        userLoggedIn = false;
                    }
                }
            } else if (userChoice == 4) {
                break;
            }
        } else {
            if (adminLoggedIn) {
                cout << "Admin Menu\n";
                cout << "1. View All User Accounts\n2. Logout\n";
                cin >> optionChoice;
                if (optionChoice == 1) {
                    ifstream userFile("user_data.txt");
                    if (userFile.is_open()) {
                        string line;
                        while (getline(userFile, line)) {
                            cout << line << endl;
                        }
                        userFile.close();
                    }
                }
                if (optionChoice == 2) {
                    userLoggedIn = false;
                }
            } else {
                cout << "User Menu:\n";
                cout << "1. Withdraw Funds\n2. Deposit Funds\n3. Account Summary\n4. Close Account\n5. Logout\n";
                cin >> optionChoice;
                if (optionChoice == 1) {
                    cout << "Enter withdrawal amount: ";
                    cin >> withdrawalAmount;
                    account->withdrawFunds(withdrawalAmount);
                } else if (optionChoice == 2) {
                    cout << "Enter deposit amount: ";
                    cin >> depositAmount;
                    account->depositFunds(depositAmount);
                } else if (optionChoice == 3) {
                    account->displayAccountDetails();
                } else if (optionChoice == 4) {
                    account->closeAccount();
                    delete account;
                    userLoggedIn = false;
                } else if (optionChoice == 5) {
                    account->saveAccountData();
                    delete account; 
                    userLoggedIn = false;
                }
            }
        }
    }
    return 0;
}




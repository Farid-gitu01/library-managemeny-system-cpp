/**
 * ============================================================================
 * Project: Advanced Console Library Management System
 * Language: C++ (C++11 or higher standard)
 * Description: An interactive, file-persistent, object-oriented system for 
 * handling library CRUD operations, issue/return pipelines, 
 * and analytical metrics tracking.
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <limits>

using namespace std;

// ============================================================================
// GLOBAL UTILITIES & DATA SANITIZATION
// ============================================================================

/**
 * Robust input verification template ensuring user inputs are of explicit type
 */
template <typename T>
T getValidatedInput(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "❌ Invalid input type. Please try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

/**
 * Specialized string input harvester handling space sequences securely
 */
string getStringInput(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    // Trim basic leading/trailing spaces if necessary
    return value;
}

// ============================================================================
// OOP BASE SYSTEM: COMPONENT ABSTRACT CORES
// ============================================================================

/**
 * Base Abstract Class defining core properties for system entities
 */
class LibraryEntity {
protected:
    string id;
    string name;
public:
    LibraryEntity() : id(""), name("") {}
    LibraryEntity(string i, string n) : id(i), name(n) {}
    virtual ~LibraryEntity() {}

    // Pure Virtual Function demonstrating abstraction/polymorphism interface
    virtual void displayDetails() const = 0;

    // Getters and Setters
    string getId() const { return id; }
    string getName() const { return name; }
    void setId(string i) { id = i; }
    void setName(string n) { name = n; }
};

// ============================================================================
// CLASS: BOOK ENTITY MODEL
// ============================================================================
class Book : public LibraryEntity {
private:
    string author;
    bool isIssued;
public:
    // Constructors
    Book() : LibraryEntity(), author(""), isIssued(false) {}
    Book(string id, string title, string auth, bool issued = false) 
        : LibraryEntity(id, title), author(auth), isIssued(issued) {}

    // Getter / Setter Overrides
    string getAuthor() const { return author; }
    bool getIssuedStatus() const { return isIssued; }
    void setAuthor(string auth) { author = auth; }
    void setIssuedStatus(bool status) { isIssued = status; }

    // Concrete override displaying unique format matrix layout
    void displayDetails() const override {
        cout << left << setw(10) << id 
             << setw(30) << name 
             << setw(25) << author 
             << setw(15) << (isIssued ? "Issued 🛑" : "Available ✅") << "\n";
    }

    /**
     * Serializes object contents into clean comma-delimited strings
     */
    string serialize() const {
        return id + "," + name + "," + author + "," + (isIssued ? "1" : "0");
    }

    /**
     * Deserializes dynamic data streams back into a structural object instance
     */
    static Book deserialize(const string& data) {
        stringstream ss(data);
        string id, title, author, issuedStr;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, issuedStr, ',');
        return Book(id, title, author, issuedStr == "1");
    }
};

// ============================================================================
// CLASS: MEMBER ENTITY MODEL
// ============================================================================
class Member : public LibraryEntity {
private:
    string email;
public:
    // Constructors
    Member() : LibraryEntity(), email("") {}
    Member(string id, string name, string mail) : LibraryEntity(id, name), email(mail) {}

    string getEmail() const { return email; }
    void setEmail(string mail) { email = mail; }

    void displayDetails() const override {
        cout << left << setw(10) << id 
             << setw(30) << name 
             << setw(30) << email << "\n";
    }

    string serialize() const {
        return id + "," + name + "," + email;
    }

    static Member deserialize(const string& data) {
        stringstream ss(data);
        string id, name, email;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, email, ',');
        return Member(id, name, email);
    }
};

// ============================================================================
// CLASS: TRANSACTION ISSUE RECORD MODEL
// ============================================================================
class IssueRecord {
private:
    string bookId;
    string memberId;
    string issueDate;
public:
    IssueRecord() : bookId(""), memberId(""), issueDate("") {}
    IssueRecord(string bId, string mId, string date) : bookId(bId), memberId(mId), issueDate(date) {}

    string getBookId() const { return bookId; }
    string getMemberId() const { return memberId; }
    string getIssueDate() const { return issueDate; }

    string serialize() const {
        return bookId + "," + memberId + "," + issueDate;
    }

    static IssueRecord deserialize(const string& data) {
        stringstream ss(data);
        string bId, mId, date;
        getline(ss, bId, ',');
        getline(ss, mId, ',');
        getline(ss, date, ',');
        return IssueRecord(bId, mId, date);
    }
};

// ============================================================================
// CLASS: DATA STORES ARCHITECTURE MANAGER (CRUD ENGINE)
// ============================================================================
class LibraryManager {
private:
    vector<Book> books;
    vector<Member> members;
    vector<IssueRecord> issues;

    const string BOOKS_FILE = "books.dat";
    const string MEMBERS_FILE = "members.dat";
    const string ISSUES_FILE = "issued_books.dat";

    // --- Private Helper Internal Storage Access Modifiers ---
    void loadData() {
        string line;
        
        ifstream bFile(BOOKS_FILE);
        if (bFile.is_open()) {
            books.clear();
            while (getline(bFile, line)) {
                if(!line.empty()) books.push_back(Book::deserialize(line));
            }
            bFile.close();
        }

        ifstream mFile(MEMBERS_FILE);
        if (mFile.is_open()) {
            members.clear();
            while (getline(mFile, line)) {
                if(!line.empty()) members.push_back(Member::deserialize(line));
            }
            mFile.close();
        }

        ifstream iFile(ISSUES_FILE);
        if (iFile.is_open()) {
            issues.clear();
            while (getline(iFile, line)) {
                if(!line.empty()) issues.push_back(IssueRecord::deserialize(line));
            }
            iFile.close();
        }
    }

public:
    LibraryManager() {
        loadData();
    }

    void saveAllData() {
        ofstream bFile(BOOKS_FILE, ios::trunc);
        for (const auto& b : books) bFile << b.serialize() << "\n";
        
        ofstream mFile(MEMBERS_FILE, ios::trunc);
        for (const auto& m : members) mFile << m.serialize() << "\n";

        ofstream iFile(ISSUES_FILE, ios::trunc);
        for (const auto& i : issues) iFile << i.serialize() << "\n";
    }

    // ========================================================================
    // BOOK SUB-CRUD FUNCTIONALITIES
    // ========================================================================
    
    void addBook() {
        cout << "\n--- Add New Book ---\n";
        string id = getStringInput("Enter Book ID: ");
        
        // Uniqueness validation check
        for(const auto& b : books) {
            if(b.getId() == id) {
                cout << "❌ Error: A book with this ID already exists.\n";
                return;
            }
        }
        string title = getStringInput("Enter Book Title: ");
        string author = getStringInput("Enter Author Name: ");

        books.push_back(Book(id, title, author));
        saveAllData();
        cout << "🚀 Book added to catalog successfully.\n";
    }

    void viewAllBooks() const {
        cout << "\n--------------------------------------------------------------------------------\n";
        cout << left << setw(10) << "Book ID" << setw(30) << "Title" << setw(25) << "Author" << setw(15) << "Status" << "\n";
        cout << "--------------------------------------------------------------------------------\n";
        if(books.empty()) cout << "   No entries logged in database.\n";
        for (const auto& b : books) b.displayDetails();
        cout << "--------------------------------------------------------------------------------\n";
    }

    void searchBook() const {
        cout << "\nSearch Book By: 1. ID | 2. Title | 3. Author\n";
        int choice = getValidatedInput<int>("Select option: ");
        string criteria = getStringInput("Enter search keywords: ");
        transform(criteria.begin(), criteria.end(), criteria.begin(), ::tolower);

        cout << "\n--------------------------------------------------------------------------------\n";
        cout << left << setw(10) << "Book ID" << setw(30) << "Title" << setw(25) << "Author" << setw(15) << "Status" << "\n";
        cout << "--------------------------------------------------------------------------------\n";

        bool found = false;
        for (const auto& b : books) {
            string target = "";
            if (choice == 1) target = b.getId();
            else if (choice == 2) target = b.getName();
            else if (choice == 3) target = b.getAuthor();

            transform(target.begin(), target.end(), target.begin(), ::tolower);
            if (target.find(criteria) != string::npos) {
                b.displayDetails();
                found = true;
            }
        }
        if(!found) cout << "❌ No matching records found.\n";
        cout << "--------------------------------------------------------------------------------\n";
    }

    void updateBook() {
        string id = getStringInput("\nEnter Book ID to update: ");
        for (auto& b : books) {
            if (b.getId() == id) {
                cout << "Current Info: ";
                b.displayDetails();
                b.setName(getStringInput("Enter New Title (or hit enter to keep old): "));
                b.setAuthor(getStringInput("Enter New Author Name: "));
                saveAllData();
                cout << "🔄 Book update records locked.\n";
                return;
            }
        }
        cout << "❌ Book code mapping not found.\n";
    }

    void deleteBook() {
        string id = getStringInput("\nEnter Book ID to remove: ");
        auto it = remove_if(books.begin(), books.end(), [&id](const Book& b) {
            if(b.getId() == id) {
                if(b.getIssuedStatus()) {
                    cout << "⚠️ Cannot delete book while active transaction holds status 'Issued'.\n";
                    return false;
                }
                return true;
            }
            return false;
        });

        if (it != books.end()) {
            books.erase(it, books.end());
            saveAllData();
            cout << "🗑️ Book record eradicated from database mapping.\n";
        } else {
            cout << "❌ Action aborted. Book ID mismatch or actively checked out.\n";
        }
    }

    // ========================================================================
    // MEMBER SUB-CRUD FUNCTIONALITIES
    // ========================================================================
    
    void addMember() {
        cout << "\n--- Enroll New Member ---\n";
        string id = getStringInput("Generate/Enter Member ID: ");
        for(const auto& m : members) {
            if(m.getId() == id) {
                cout << "❌ Error: Profile registration collides with an existing ID key.\n";
                return;
            }
        }
        string name = getStringInput("Enter Member Full Name: ");
        string email = getStringInput("Enter Communications Email: ");

        members.push_back(Member(id, name, email));
        saveAllData();
        cout << "💳 Account profile active. Member registered.\n";
    }

    void viewAllMembers() const {
        cout << "\n----------------------------------------------------------------------\n";
        cout << left << setw(10) << "User ID" << setw(30) << "Full Name" << setw(30) << "Email Address" << "\n";
        cout << "----------------------------------------------------------------------\n";
        if(members.empty()) cout << "   No active accounts present.\n";
        for (const auto& m : members) m.displayDetails();
        cout << "----------------------------------------------------------------------\n";
    }

    void searchMember() const {
        string token = getStringInput("\nEnter Member ID or profile name: ");
        transform(token.begin(), token.end(), token.begin(), ::tolower);

        cout << "\n----------------------------------------------------------------------\n";
        cout << left << setw(10) << "User ID" << setw(30) << "Full Name" << setw(30) << "Email Address" << "\n";
        cout << "----------------------------------------------------------------------\n";

        bool match = false;
        for (const auto& m : members) {
            string idLower = m.getId();
            string nameLower = m.getName();
            transform(idLower.begin(), idLower.end(), idLower.begin(), ::tolower);
            transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

            if (idLower.find(token) != string::npos || nameLower.find(token) != string::npos) {
                m.displayDetails();
                match = true;
            }
        }
        if(!match) cout << "❌ No system users correlate with input criteria parameters.\n";
        cout << "----------------------------------------------------------------------\n";
    }

    void updateMember() {
        string id = getStringInput("\nEnter Member ID to modify: ");
        for (auto& m : members) {
            if (m.getId() == id) {
                m.setName(getStringInput("Update Name: "));
                m.setEmail(getStringInput("Update Email: "));
                saveAllData();
                cout << "⚙️ Profile data sync structural changes saved.\n";
                return;
            }
        }
        cout << "❌ User database match returned null reference pointer.\n";
    }

    void deleteMember() {
        string id = getStringInput("\nEnter Member ID to terminate: ");
        
        // Prevent deleting members with outstanding book returns
        for(const auto& i : issues) {
            if(i.getMemberId() == id) {
                cout << "❌ Termination denied. Account balance shows active item distribution pipelines.\n";
                return;
            }
        }

        auto it = remove_if(members.begin(), members.end(), [&id](const Member& m) { return m.getId() == id; });
        if (it != members.end()) {
            members.erase(it, members.end());
            saveAllData();
            cout << "🗑️ User account configuration wiped clean.\n";
        } else {
            cout << "❌ Processing Error: ID vector allocation path not located.\n";
        }
    }

    // ========================================================================
    // TRANSACTION HANDLERS (ISSUE/RETURN PIPELINE)
    // ========================================================================
    
    void issueBook() {
        cout << "\n--- Book Checkout Distribution Processing ---\n";
        string bId = getStringInput("Scan/Enter targeted Book ID: ");
        
        // Locate book record pointer
        Book* targetBook = nullptr;
        for(auto& b : books) {
            if(b.getId() == bId) {
                targetBook = &b;
                break;
            }
        }

        if(!targetBook) {
            cout << "❌ Catalog reference match failure. System does not hold matching Book ID.\n";
            return;
        }
        if(targetBook->getIssuedStatus()) {
            cout << "🔒 Transaction exception: Target item distribution state is already marked 'Issued'.\n";
            return;
        }

        string mId = getStringInput("Scan/Enter Member ID: ");
        bool memberValid = false;
        for(const auto& m : members) {
            if(m.getId() == mId) {
                memberValid = true;
                break;
            }
        }

        if(!memberValid) {
            cout << "❌ Operation rejected. Invalid transaction validation reference parameters.\n";
            return;
        }

        string date = getStringInput("Enter Transaction Timestamp Date (YYYY-MM-DD): ");

        // Perform transactional mutation updates
        targetBook->setIssuedStatus(true);
        issues.push_back(IssueRecord(bId, mId, date));
        saveAllData();
        cout << "✨ Checkout process initialized successfully. Book assigned.\n";
    }

    void returnBook() {
        cout << "\n--- Inventory Item Return Reclamation System ---\n";
        string bId = getStringInput("Scan/Enter returned Book ID: ");

        Book* targetBook = nullptr;
        for(auto& b : books) {
            if(b.getId() == bId) {
                targetBook = &b;
                break;
            }
        }

        if(!targetBook || !targetBook->getIssuedStatus()) {
            cout << "❌ Error: Asset check indicates index item is not currently listed as distributed.\n";
            return;
        }

        // Wipe transaction record matching parameters
        auto it = remove_if(issues.begin(), issues.end(), [&bId](const IssueRecord& r) { return r.getBookId() == bId; });
        if (it != issues.end()) {
            issues.erase(it, issues.end());
            targetBook->setIssuedStatus(false);
            saveAllData();
            cout << "📦 Quality checks passed. Catalog balance logs reconciled back to shelf allocation values.\n";
        } else {
            cout << "⚠️ State disparity detected: Adjusting structural flags to default overrides.\n";
            targetBook->setIssuedStatus(false);
            saveAllData();
        }
    }

    // ========================================================================
    // ANALYTICAL METRICS REPORTING DASHBOARD
    // ========================================================================
    
    void generateSystemReports() const {
        size_t totalBooks = books.size();
        size_t issuedBooks = count_if(books.begin(), books.end(), [](const Book& b) { return b.getIssuedStatus(); });
        size_t availableBooks = totalBooks - issuedBooks;
        size_t totalMembers = members.size();

        cout << "\n==================================================================\n";
        cout << "                 STATISTICAL CORE SYSTEM METRICS                  \n";
        cout << "==================================================================\n";
        cout << " 📈 Total Registered Book Copies: " << totalBooks << "\n";
        cout << " 🟢 Available Books On Shelves   : " << availableBooks << "\n";
        cout << " 🔴 Active Items Circulating    : " << issuedBooks << "\n";
        cout << " 👥 Total Active Library Patrons : " << totalMembers << "\n";
        cout << "==================================================================\n";
        cout << "            CHRONOLOGICAL RECENT SYSTEM TRANSACTION LOG           \n";
        cout << "------------------------------------------------------------------\n";
        cout << left << setw(15) << "Book Code" << setw(15) << "User Profile" << setw(15) << "Registry Date" << "\n";
        
        if(issues.empty()) cout << " [No systemic transaction entries actively processing values]\n";
        
        // Show historical indices backwards
        size_t displayCounter = 0;
        for(auto rit = issues.rbegin(); rit != issues.rend() && displayCounter < 5; ++rit, ++displayCounter) {
            cout << left << setw(15) << rit->getBookId() 
                 << setw(15) << rit->getMemberId() 
                 << setw(15) << rit->getIssueDate() << "\n";
        }
        cout << "==================================================================\n";
    }
};

// ============================================================================
// CONSOLE VIEW INTERACTIVE LAYER MANAGEMENT ENGINE
// ============================================================================

void runBookManagementMenu(LibraryManager& lm) {
    while (true) {
        cout << "\n📖 Catalog Control Panel\n"
             << "1. Add New Asset Record\n"
             << "2. View Document Indices\n"
             << "3. Search Registry Array\n"
             << "4. Modify Existing Details\n"
             << "5. Terminate Index Reference\n"
             << "6. Back to Root Menu\n";
        int choice = getValidatedInput<int>("Select operation path index: ");
        switch (choice) {
            case 1: lm.addBook(); break;
            case 2: lm.viewAllBooks(); break;
            case 3: lm.searchBook(); break;
            case 4: lm.updateBook(); break;
            case 5: lm.deleteBook(); break;
            case 6: return;
            default: cout << "❌ System Option out of bounds range indices.\n";
        }
    }
}

void runMemberManagementMenu(LibraryManager& lm) {
    while (true) {
        cout << "\n👥 Patron Registry Control Panel\n"
             << "1. Register New Member Profile\n"
             << "2. View Active Enrolled base\n"
             << "3. Locate Profile Reference\n"
             << "4. Re-calibrate Demographic Metrics\n"
             << "5. De-authorize Member Accounts\n"
             << "6. Return to Root Menu\n";
        int choice = getValidatedInput<int>("Select operation path index: ");
        switch (choice) {
            case 1: lm.addMember(); break;
            case 2: lm.viewAllMembers(); break;
            case 3: lm.searchMember(); break;
            case 4: lm.updateMember(); break;
            case 5: lm.deleteMember(); break;
            case 6: return;
            default: cout << "❌ System Option out of bounds range indices.\n";
        }
    }
}

// ============================================================================
// PROGRAM ENTRY ROUTINE ENTRYPOINT
// ============================================================================
int main() {
    cout << "==================================================================\n";
    cout << "     SYSTEM INTERFACE: CORE MANAGEMENT ARCHITECTURE VERSION 2.5   \n";
    cout << "==================================================================\n";

    LibraryManager lm;

    while (true) {
        cout << "\n🎛️  System Management Core Menu\n"
             << "1. Asset Management Module\n"
             << "2. Account Management Module\n"
             << "3. Execute Distribution Checkout (Issue)\n"
             << "4. Process Inventory Check-in (Return)\n"
             << "5. Operational System Analytics Report\n"
             << "6. Safely Save & Terminate Session\n";
        
        int mainChoice = getValidatedInput<int>("Direct system operations entry target: ");
        
        switch (mainChoice) {
            case 1: runBookManagementMenu(lm); break;
            case 2: runMemberManagementMenu(lm); break;
            case 3: lm.issueBook(); break;
            case 4: lm.returnBook(); break;
            case 5: lm.generateSystemReports(); break;
            case 6:
                cout << "\n💾 Initiating systemic payload snapshot updates to disk files...";
                lm.saveAllData();
                cout << "\n✨ Context pipeline variables saved safely. Shutting down. Goodbye!\n";
                return 0;
            default:
                cout << "❌ Unrecognized execution operation entry criteria parsed.\n";
        }
    }
}
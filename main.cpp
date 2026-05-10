/*
 * ================================================================
 *         BANK MANAGEMENT SYSTEM  v2.0  — C++ OOP + File I/O
 * ================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <cctype>

using namespace std;

// ================================================================
//  ANSI COLOUR PALETTE
// ================================================================
namespace C {
    const string R    = "\033[0m";
    const string BLD  = "\033[1m";
    const string DIM  = "\033[2m";
    const string UL   = "\033[4m";
    // Foreground
    const string RED  = "\033[31m";
    const string GRN  = "\033[32m";
    const string YLW  = "\033[33m";
    const string BLU  = "\033[34m";
    const string MAG  = "\033[35m";
    const string CYN  = "\033[36m";
    const string WHT  = "\033[37m";
    // Bright foreground
    const string BRED = "\033[91m";
    const string BGRN = "\033[92m";
    const string BYLW = "\033[93m";
    const string BBLU = "\033[94m";
    const string BMAG = "\033[95m";
    const string BCYN = "\033[96m";
    const string BWHT = "\033[97m";
    // Background
    const string BG_BLU = "\033[44m";
    const string BG_GRN = "\033[42m";
    const string BG_RED = "\033[41m";
    const string BG_YLW = "\033[43m";
    const string BG_CYN = "\033[46m";
    const string BG_MAG = "\033[45m";
}

// ================================================================
//  UTILITY HELPERS
// ================================================================
namespace Utils {

    string currentTimestamp() {
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
        return string(buf);
    }

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void pauseForEnter() {
        cout << "\n" << C::DIM << C::YLW
             << "  Press Enter to return to the menu..." << C::R;
        cin.clear();
        string dummy;
        getline(cin, dummy);
    }

    // ── Validators ─────────────────────────────────────────────

    // User PIN: 4-6 numeric digits only
    bool isValidPin(const string& pin) {
        if (pin.size() < 4 || pin.size() > 6) return false;
        return all_of(pin.begin(), pin.end(), ::isdigit);
    }

    // Admin PIN: 6-12 chars, must have >= 1 letter AND >= 1 digit
    bool isValidAdminPin(const string& pin) {
        if (pin.size() < 6 || pin.size() > 12) return false;
        bool hasLetter = any_of(pin.begin(), pin.end(), ::isalpha);
        bool hasDigit  = any_of(pin.begin(), pin.end(), ::isdigit);
        bool allAlnum  = all_of(pin.begin(), pin.end(),
                                [](char c){ return isalnum(c) || c == '@' || c == '_'; });
        return hasLetter && hasDigit && allAlnum;
    }

    bool isValidName(const string& name) {
        if (name.size() < 2 || name.size() > 50) return false;
        return all_of(name.begin(), name.end(),
                      [](char c){ return isalpha(c) || c == ' '; });
    }

    bool isValidAccountId(const string& id) {
        if (id.size() < 4) return false;
        if (id.substr(0, 3) != "ACC") return false;
        return all_of(id.begin() + 3, id.end(), ::isdigit);
    }

    // ── Styled Readers ──────────────────────────────────────────

    double readPositiveDouble(const string& prompt) {
        double val;
        while (true) {
            cout << C::YLW << prompt << C::R;
            if (cin >> val && val > 0.0) { clearInput(); return val; }
            cout << C::BRED << "  [!] Enter a valid positive amount.\n" << C::R;
            clearInput();
        }
    }

    int readIntInRange(const string& prompt, int lo, int hi) {
        int val;
        while (true) {
            cout << C::YLW << prompt << C::R;
            if (cin >> val && val >= lo && val <= hi) { clearInput(); return val; }
            cout << C::BRED << "  [!] Choose between " << lo << " and " << hi << ".\n" << C::R;
            clearInput();
        }
    }

    string readValidatedPin(const string& prompt) {
        while (true) {
            cout << C::YLW << prompt << C::R;
            string pin; cin >> pin; clearInput();
            if (isValidPin(pin)) return pin;
            cout << C::BRED << "  [!] PIN must be 4-6 numeric digits only.\n" << C::R;
        }
    }

    string readValidatedName(const string& prompt) {
        while (true) {
            cout << C::YLW << prompt << C::R;
            string name; getline(cin, name);
            name.erase(name.find_last_not_of(" \t\r\n") + 1);
            if (isValidName(name)) return name;
            cout << C::BRED << "  [!] Name must be 2-50 letters/spaces only.\n" << C::R;
        }
    }

    string readValidatedAccountId(const string& prompt) {
        while (true) {
            cout << C::YLW << prompt << C::R;
            string id; cin >> id; clearInput();
            if (isValidAccountId(id)) return id;
            cout << C::BRED << "  [!] Invalid format. Account IDs look like ACC1001.\n" << C::R;
        }
    }

    // Admin PIN reader: letters + digits, 6-12 chars
    string readValidatedAdminPin(const string& prompt) {
        while (true) {
            cout << C::YLW << prompt << C::R;
            string pin; cin >> pin; clearInput();
            if (isValidAdminPin(pin)) return pin;
            cout << C::BRED
                 << "  [!] Admin PIN must be 6-12 chars with at least one letter and one digit.\n"
                 << C::R;
        }
    }

    // ── Visual Helpers ──────────────────────────────────────────

    void separator(const string& colour = C::BCYN, char c = '-', int n = 64) {
        cout << colour << string(n, c) << C::R << "\n";
    }

    void printTitle(const string& title,
                    const string& fg = C::BLD + C::BWHT,
                    const string& bg = C::BG_BLU, int width = 64) {
        int pad = max(0, (width - (int)title.size()) / 2);
        cout << bg << fg
             << string(pad, ' ') << title
             << string(width - pad - (int)title.size(), ' ')
             << C::R << "\n";
    }

    void printSuccess(const string& msg) {
        cout << C::BGRN << C::BLD << "  [OK] " << C::R
             << C::BGRN << msg << C::R << "\n";
    }

    void printError(const string& msg) {
        cout << C::BRED << C::BLD << "  [!!] " << C::R
             << C::BRED << msg << C::R << "\n";
    }

    void printInfo(const string& label, const string& value,
                   const string& lc = C::BCYN,
                   const string& vc = C::BWHT) {
        cout << "  " << lc << left << setw(18) << label
             << C::R << ": " << vc << value << C::R << "\n";
    }

    string fmtMoney(double v) {
        ostringstream s;
        s << fixed << setprecision(2) << v;
        return s.str();
    }
}

// ================================================================
//  TRANSACTION
// ================================================================
class Transaction {
public:
    enum class Type { DEPOSIT, WITHDRAWAL, TRANSFER_IN, TRANSFER_OUT, OPEN };

    Type   type;
    double amount;
    double balanceAfter;
    string timestamp;
    string note;

    Transaction() = default;
    Transaction(Type t, double amt, double bal, const string& n = "")
        : type(t), amount(amt), balanceAfter(bal),
          timestamp(Utils::currentTimestamp()), note(n) {}

    static string typeLabel(Type t) {
        switch (t) {
            case Type::DEPOSIT:      return "DEPOSIT     ";
            case Type::WITHDRAWAL:   return "WITHDRAWAL  ";
            case Type::TRANSFER_IN:  return "TRANSFER IN ";
            case Type::TRANSFER_OUT: return "TRANSFER OUT";
            case Type::OPEN:         return "ACCT OPENED ";
            default:                 return "UNKNOWN     ";
        }
    }

    static string typeColour(Type t) {
        switch (t) {
            case Type::DEPOSIT:      return C::BGRN;
            case Type::WITHDRAWAL:   return C::BRED;
            case Type::TRANSFER_IN:  return C::BCYN;
            case Type::TRANSFER_OUT: return C::BYLW;
            case Type::OPEN:         return C::BMAG;
            default:                 return C::BWHT;
        }
    }

    string serialise() const {
        return to_string((int)type) + "|" +
               to_string(amount)   + "|" +
               to_string(balanceAfter) + "|" +
               timestamp + "|" + note;
    }

    static Transaction deserialise(const string& line) {
        Transaction tx;
        istringstream ss(line);
        string token; int idx = 0;
        while (getline(ss, token, '|')) {
            switch (idx++) {
                case 0: tx.type         = static_cast<Type>(stoi(token)); break;
                case 1: tx.amount       = stod(token); break;
                case 2: tx.balanceAfter = stod(token); break;
                case 3: tx.timestamp    = token;        break;
                case 4: tx.note         = token;        break;
            }
        }
        return tx;
    }

    void print() const {
        string col = typeColour(type);
        cout << "  " << C::DIM  << timestamp << C::R
             << "  " << col << C::BLD << typeLabel(type) << C::R
             << "  " << C::BMAG << right << setw(11)
             << fixed << setprecision(2) << amount << C::R
             << "  " << C::DIM  << "Bal:" << C::R
             << " " << C::BWHT  << setw(11) << balanceAfter << C::R;
        if (!note.empty())
            cout << "  " << C::DIM << "[" << note << "]" << C::R;
        cout << "\n";
    }
};

// ================================================================
//  ACCOUNT  (abstract base)
// ================================================================
class Account {
protected:
    string              accountId;
    string              holderName;
    string              pin;
    double              balance;
    string              createdAt;
    vector<Transaction> history;

public:
    Account() : balance(0.0) {}
    Account(const string& id, const string& name,
            const string& p, double init)
        : accountId(id), holderName(name), pin(p),
          balance(init), createdAt(Utils::currentTimestamp())
    {
        history.emplace_back(Transaction::Type::OPEN, init, init, "Account opened");
    }
    virtual ~Account() = default;

    const string& getId()      const { return accountId;  }
    const string& getName()    const { return holderName; }
    double        getBalance() const { return balance;    }
    const string& getCreated() const { return createdAt;  }
    bool verifyPin(const string& p) const { return pin == p; }

    virtual string accountType()  const = 0;
    virtual double interestRate() const = 0;
    virtual string typeColour()   const = 0;

    virtual bool deposit(double amount, const string& note = "") {
        if (amount <= 0) throw invalid_argument("Deposit amount must be positive.");
        balance += amount;
        history.emplace_back(Transaction::Type::DEPOSIT, amount, balance, note);
        return true;
    }

    virtual bool withdraw(double amount, const string& note = "") {
        if (amount <= 0) throw invalid_argument("Withdrawal amount must be positive.");
        if (amount > balance)
            throw runtime_error("Insufficient funds. Available: INR " + Utils::fmtMoney(balance));
        balance -= amount;
        history.emplace_back(Transaction::Type::WITHDRAWAL, amount, balance, note);
        return true;
    }

    void recordTransferIn(double amount, const string& fromId) {
        balance += amount;
        history.emplace_back(Transaction::Type::TRANSFER_IN, amount, balance, "From " + fromId);
    }

    void recordTransferOut(double amount, const string& toId) {
        if (amount > balance)
            throw runtime_error("Insufficient funds for transfer.");
        balance -= amount;
        history.emplace_back(Transaction::Type::TRANSFER_OUT, amount, balance, "To " + toId);
    }

    virtual void printSummary() const {
        string col = typeColour();
        Utils::separator(col, '=');
        Utils::printTitle("  ACCOUNT SUMMARY  ", C::BLD + C::BWHT, C::BG_BLU);
        Utils::separator(col, '=');
        Utils::printInfo("Account ID",   accountId,   C::BCYN, C::BWHT + C::BLD);
        Utils::printInfo("Holder Name",  holderName,  C::BCYN, C::BWHT);
        Utils::printInfo("Account Type", accountType(),C::BCYN, col + C::BLD);
        Utils::printInfo("Balance",      "INR " + Utils::fmtMoney(balance), C::BCYN, C::BGRN + C::BLD);
        Utils::printInfo("Interest Rate",Utils::fmtMoney(interestRate() * 100) + "% p.a.", C::BCYN, C::BYLW);
        Utils::printInfo("Opened On",    createdAt,   C::BCYN, C::DIM + C::BWHT);
        Utils::separator(col, '=');
    }

    void printHistory() const {
        Utils::separator(C::BMAG, '=');
        Utils::printTitle("  TRANSACTION HISTORY  ", C::BLD + C::BWHT, C::BG_MAG);
        Utils::separator(C::BMAG, '=');
        cout << "  " << C::BCYN << accountId << C::R
             << "  " << C::BWHT << C::BLD << holderName << C::R << "\n";
        Utils::separator(C::DIM);
        if (history.empty()) {
            cout << C::DIM << "  No transactions recorded yet.\n" << C::R;
        } else {
            cout << "  " << C::DIM
                 << left  << setw(19) << "Date & Time"
                 << "  "  << setw(14) << "Type"
                 << "  "  << right << setw(11) << "Amount"
                 << "  "  << setw(13) << "Balance After"
                 << C::R  << "\n";
            Utils::separator(C::DIM, '.');
            for (const auto& tx : history) tx.print();
        }
        Utils::separator(C::BMAG, '=');
    }

    void restoreFields(const string& id, const string& name,
                       const string& p, double bal, const string& created) {
        accountId = id;    holderName = name;
        pin       = p;     balance    = bal;
        createdAt = created;
    }

    void loadHistory(ifstream& in, size_t count) {
        history.clear();
        string line;
        for (size_t i = 0; i < count; ++i) {
            if (!getline(in, line)) break;
            if (line.rfind("TX|", 0) == 0)
                history.push_back(Transaction::deserialise(line.substr(3)));
        }
    }

    virtual void saveToFile(ofstream& out) const {
        out << "ACCOUNT|" << accountType() << "|" << accountId << "|"
            << holderName << "|" << pin << "|"
            << fixed << setprecision(6) << balance << "|"
            << createdAt << "\n";
        out << "TXCOUNT|" << history.size() << "\n";
        for (const auto& tx : history)
            out << "TX|" << tx.serialise() << "\n";
        out << "END\n";
    }
};

// ================================================================
//  ACCOUNT TYPES
// ================================================================
class SavingsAccount : public Account {
    static constexpr double RATE    = 0.04;
    static constexpr double MIN_BAL = 500.0;
public:
    SavingsAccount() = default;
    SavingsAccount(const string& id, const string& name,
                   const string& p, double init)
        : Account(id, name, p, init) {}

    string accountType()  const override { return "Savings";  }
    double interestRate() const override { return RATE;        }
    string typeColour()   const override { return C::BGRN;    }

    bool withdraw(double amount, const string& note = "") override {
        if (balance - amount < MIN_BAL)
            throw runtime_error(
                "Savings accounts must maintain a minimum balance of INR 500.00.");
        return Account::withdraw(amount, note);
    }
};

class CurrentAccount : public Account {
    static constexpr double RATE      = 0.02;
    static constexpr double OVERDRAFT = 5000.0;
public:
    CurrentAccount() = default;
    CurrentAccount(const string& id, const string& name,
                   const string& p, double init)
        : Account(id, name, p, init) {}

    string accountType()  const override { return "Current";  }
    double interestRate() const override { return RATE;        }
    string typeColour()   const override { return C::BBLU;    }

    bool withdraw(double amount, const string& note = "") override {
        if (amount <= 0) throw invalid_argument("Amount must be positive.");
        if (amount > balance + OVERDRAFT)
            throw runtime_error("Exceeds overdraft limit of INR 5,000.00.");
        balance -= amount;
        history.emplace_back(Transaction::Type::WITHDRAWAL, amount, balance, note);
        return true;
    }
};

class FixedDepositAccount : public Account {
    static constexpr double RATE = 0.065;
public:
    FixedDepositAccount() = default;
    FixedDepositAccount(const string& id, const string& name,
                        const string& p, double init)
        : Account(id, name, p, init) {}

    string accountType()  const override { return "FixedDeposit"; }
    double interestRate() const override { return RATE;            }
    string typeColour()   const override { return C::BYLW;        }

    bool deposit(double /*a*/, const string& /*n*/ = "") override {
        throw runtime_error("Fixed Deposit accounts do not accept additional deposits.");
    }
    bool withdraw(double /*a*/, const string& /*n*/ = "") override {
        throw runtime_error(
            "Fixed Deposit accounts cannot be withdrawn before maturity.\n"
            "     Contact a branch manager to close the FD early.");
    }
};

// ================================================================
//  TRASH ENTRY
// ================================================================
struct TrashedEntry {
    string              accType, accountId, holderName, pin;
    double              balance    = 0.0;
    string              createdAt, deletedAt;
    time_t              deletedTs  = 0;
    vector<Transaction> history;

    int  daysLeft()  const {
        int elapsed = (int)((time(nullptr) - deletedTs) / 86400);
        return max(0, 30 - elapsed);
    }
    bool isExpired() const { return daysLeft() == 0; }

    void save(ofstream& out) const {
        out << "TRASH|" << accType << "|" << accountId << "|"
            << holderName << "|" << pin << "|"
            << fixed << setprecision(6) << balance << "|"
            << createdAt << "|" << deletedAt << "|" << (long long)deletedTs << "\n";
        out << "TXCOUNT|" << history.size() << "\n";
        for (const auto& tx : history)
            out << "TX|" << tx.serialise() << "\n";
        out << "END\n";
    }
};

// ================================================================
//  BANK
// ================================================================
class Bank {
    const string DATA_FILE  = "bank_data.dat";
    const string TRASH_FILE = "bank_trash.dat";
    const string LOG_FILE   = "bank_log.txt";
    const string ADMIN_PIN  = "Bank@2025";

    vector<Account*>     accounts;
    vector<TrashedEntry> trash;
    int nextId = 1001;

    // ── Helpers ──────────────────────────────────────────────────

    string generateId() { return "ACC" + to_string(nextId++); }

    Account* findAccount(const string& id) {
        for (auto* a : accounts)
            if (a->getId() == id) return a;
        return nullptr;
    }

    void log(const string& msg) {
        ofstream lf(LOG_FILE, ios::app);
        lf << "[" << Utils::currentTimestamp() << "] " << msg << "\n";
    }

    // ── Persistence ──────────────────────────────────────────────

    void loadData() {
        ifstream in(DATA_FILE);
        if (!in.is_open()) return;
        string line;
        if (getline(in, line) && line.rfind("NEXTID|", 0) == 0)
            nextId = stoi(line.substr(7));

        while (getline(in, line)) {
            if (line.rfind("ACCOUNT|", 0) != 0) continue;
            istringstream ss(line);
            vector<string> p; string tok;
            while (getline(ss, tok, '|')) p.push_back(tok);
            if (p.size() < 7) continue;

            size_t txCount = 0;
            if (getline(in, line) && line.rfind("TXCOUNT|", 0) == 0)
                txCount = stoul(line.substr(8));

            Account* acc = nullptr;
            if      (p[1] == "Savings")      acc = new SavingsAccount();
            else if (p[1] == "Current")      acc = new CurrentAccount();
            else if (p[1] == "FixedDeposit") acc = new FixedDepositAccount();
            else                             acc = new SavingsAccount();

            acc->restoreFields(p[2], p[3], p[4], stod(p[5]), p[6]);
            acc->loadHistory(in, txCount);
            getline(in, line); // END
            accounts.push_back(acc);
        }
    }

    void saveData() const {
        ofstream out(DATA_FILE, ios::trunc);
        out << "NEXTID|" << nextId << "\n";
        for (const auto* a : accounts) a->saveToFile(out);
    }

    void loadTrash() {
        ifstream in(TRASH_FILE);
        if (!in.is_open()) return;
        string line;
        while (getline(in, line)) {
            if (line.rfind("TRASH|", 0) != 0) continue;
            istringstream ss(line);
            vector<string> p; string tok;
            while (getline(ss, tok, '|')) p.push_back(tok);
            if (p.size() < 9) continue;

            TrashedEntry e;
            e.accType    = p[1]; e.accountId  = p[2]; e.holderName = p[3];
            e.pin        = p[4]; e.balance    = stod(p[5]);
            e.createdAt  = p[6]; e.deletedAt  = p[7];
            e.deletedTs  = (time_t)stoll(p[8]);

            size_t txCount = 0;
            if (getline(in, line) && line.rfind("TXCOUNT|", 0) == 0)
                txCount = stoul(line.substr(8));

            for (size_t i = 0; i < txCount; ++i) {
                if (!getline(in, line)) break;
                if (line.rfind("TX|", 0) == 0)
                    e.history.push_back(Transaction::deserialise(line.substr(3)));
            }
            getline(in, line); // END

            if (!e.isExpired()) trash.push_back(e);
        }
    }

    void saveTrash() const {
        ofstream out(TRASH_FILE, ios::trunc);
        for (const auto& e : trash) e.save(out);
    }

    // Soft-delete: save the account to trash file then remove from active list
    void moveToTrash(Account* acc) {
        // Serialise account temporarily to extract pin + history
        ofstream tmp("__tmp__.dat", ios::trunc);
        acc->saveToFile(tmp); tmp.close();

        TrashedEntry e;
        e.accType    = acc->accountType();
        e.accountId  = acc->getId();
        e.holderName = acc->getName();
        e.balance    = acc->getBalance();
        e.createdAt  = acc->getCreated();
        e.deletedAt  = Utils::currentTimestamp();
        e.deletedTs  = time(nullptr);

        ifstream rin("__tmp__.dat");
        string line;
        if (getline(rin, line)) {
            istringstream ss(line);
            vector<string> p; string tok;
            while (getline(ss, tok, '|')) p.push_back(tok);
            if (p.size() >= 5) e.pin = p[4];
        }
        size_t txCount = 0;
        if (getline(rin, line) && line.rfind("TXCOUNT|", 0) == 0)
            txCount = stoul(line.substr(8));
        for (size_t i = 0; i < txCount; ++i) {
            if (!getline(rin, line)) break;
            if (line.rfind("TX|", 0) == 0)
                e.history.push_back(Transaction::deserialise(line.substr(3)));
        }
        rin.close();
        remove("__tmp__.dat");

        trash.push_back(e);
        saveTrash();
    }

    // ── Menu helpers ─────────────────────────────────────────────

    void menuItem(int num, const string& label,
                  const string& nc = C::BCYN,
                  const string& lc = C::BWHT) {
        cout << "  " << nc << C::BLD << "[" << num << "] "
             << C::R << lc << label << C::R << "\n";
    }

    // ================================================================
    //  CUSTOMER OPERATIONS
    // ================================================================

    void createAccount() {
        Utils::separator(C::BCYN, '=');
        Utils::printTitle("  CREATE NEW ACCOUNT  ", C::BLD + C::BWHT, C::BG_CYN);
        Utils::separator(C::BCYN, '=');

        cout << "\n"
             << "  " << C::BGRN << C::BLD << "[1]" << C::R << C::BWHT
             << " Savings Account       " << C::DIM
             << "(4.0% p.a. | Min balance INR 500)\n" << C::R
             << "  " << C::BBLU << C::BLD << "[2]" << C::R << C::BWHT
             << " Current Account       " << C::DIM
             << "(2.0% p.a. | Overdraft up to INR 5,000)\n" << C::R
             << "  " << C::BYLW << C::BLD << "[3]" << C::R << C::BWHT
             << " Fixed Deposit Account " << C::DIM
             << "(6.5% p.a. | No withdrawal before maturity)\n\n" << C::R;

        int choice = Utils::readIntInRange("  Select type [1-3]: ", 1, 3);

        string name = Utils::readValidatedName("  Full Name           : ");

        string pin1, pin2;
        do {
            pin1 = Utils::readValidatedPin("  Set PIN (4-6 digits) : ");
            pin2 = Utils::readValidatedPin("  Confirm PIN          : ");
            if (pin1 != pin2) Utils::printError("PINs do not match. Try again.");
        } while (pin1 != pin2);

        double initDeposit = Utils::readPositiveDouble("  Initial Deposit (INR): ");

        if (choice == 1 && initDeposit < 500.0) {
            Utils::printError("Savings account requires a minimum initial deposit of INR 500.");
            Utils::pauseForEnter();
            return;
        }

        string   id  = generateId();
        Account* acc = nullptr;
        switch (choice) {
            case 1: acc = new SavingsAccount(id, name, pin1, initDeposit);      break;
            case 2: acc = new CurrentAccount(id, name, pin1, initDeposit);      break;
            case 3: acc = new FixedDepositAccount(id, name, pin1, initDeposit); break;
        }

        accounts.push_back(acc);
        saveData();
        log("Account created: " + id + " (" + name + ")");

        cout << "\n";
        Utils::separator(C::BGRN, '=');
        Utils::printSuccess("Account created successfully!");
        cout << "\n";
        Utils::printInfo("Account ID",  id,                       C::BCYN, C::BWHT + C::BLD);
        Utils::printInfo("Holder",      name,                     C::BCYN, C::BWHT);
        Utils::printInfo("Type",        acc->accountType(),       C::BCYN, acc->typeColour() + C::BLD);
        Utils::printInfo("Balance",     "INR " + Utils::fmtMoney(initDeposit), C::BCYN, C::BGRN + C::BLD);
        Utils::separator(C::BGRN, '=');
        Utils::pauseForEnter();
    }

    void deposit() {
        Utils::separator(C::BGRN, '=');
        Utils::printTitle("  DEPOSIT FUNDS  ", C::BLD + C::BWHT, C::BG_GRN);
        Utils::separator(C::BGRN, '=');
        cout << "\n";

        string id = Utils::readValidatedAccountId("  Account ID  : ");
        Account* acc = findAccount(id);
        if (!acc) { Utils::printError("Account not found."); Utils::pauseForEnter(); return; }

        string pin = Utils::readValidatedPin("  PIN         : ");
        if (!acc->verifyPin(pin)) { Utils::printError("Incorrect PIN."); Utils::pauseForEnter(); return; }

        double amount = Utils::readPositiveDouble("  Amount (INR): ");

        try {
            acc->deposit(amount);
            saveData();
            log("Deposit INR " + Utils::fmtMoney(amount) + " -> " + id);
            cout << "\n";
            Utils::printSuccess("Deposited INR " + Utils::fmtMoney(amount));
            Utils::printInfo("New Balance", "INR " + Utils::fmtMoney(acc->getBalance()),
                             C::BCYN, C::BGRN + C::BLD);
        } catch (const exception& e) { Utils::printError(e.what()); }
        Utils::pauseForEnter();
    }

    void withdraw() {
        Utils::separator(C::BRED, '=');
        Utils::printTitle("  WITHDRAW FUNDS  ", C::BLD + C::BWHT, C::BG_RED);
        Utils::separator(C::BRED, '=');
        cout << "\n";

        string id = Utils::readValidatedAccountId("  Account ID  : ");
        Account* acc = findAccount(id);
        if (!acc) { Utils::printError("Account not found."); Utils::pauseForEnter(); return; }

        string pin = Utils::readValidatedPin("  PIN         : ");
        if (!acc->verifyPin(pin)) { Utils::printError("Incorrect PIN."); Utils::pauseForEnter(); return; }

        Utils::printInfo("Available Balance", "INR " + Utils::fmtMoney(acc->getBalance()),
                         C::BCYN, C::BWHT + C::BLD);
        double amount = Utils::readPositiveDouble("  Amount (INR): ");

        try {
            acc->withdraw(amount);
            saveData();
            log("Withdrawal INR " + Utils::fmtMoney(amount) + " <- " + id);
            cout << "\n";
            Utils::printSuccess("Withdrawn INR " + Utils::fmtMoney(amount));
            Utils::printInfo("New Balance", "INR " + Utils::fmtMoney(acc->getBalance()),
                             C::BCYN, C::BYLW + C::BLD);
        } catch (const exception& e) { Utils::printError(e.what()); }
        Utils::pauseForEnter();
    }

    void checkBalance() {
        Utils::separator(C::BBLU, '=');
        Utils::printTitle("  BALANCE INQUIRY  ", C::BLD + C::BWHT, C::BG_BLU);
        Utils::separator(C::BBLU, '=');
        cout << "\n";

        string id = Utils::readValidatedAccountId("  Account ID: ");
        Account* acc = findAccount(id);
        if (!acc) { Utils::printError("Account not found."); Utils::pauseForEnter(); return; }

        string pin = Utils::readValidatedPin("  PIN       : ");
        if (!acc->verifyPin(pin)) { Utils::printError("Incorrect PIN."); Utils::pauseForEnter(); return; }

        cout << "\n";
        acc->printSummary();
        Utils::pauseForEnter();
    }

    void viewHistory() {
        Utils::separator(C::BMAG, '=');
        Utils::printTitle("  TRANSACTION HISTORY  ", C::BLD + C::BWHT, C::BG_MAG);
        Utils::separator(C::BMAG, '=');
        cout << "\n";

        string id = Utils::readValidatedAccountId("  Account ID: ");
        Account* acc = findAccount(id);
        if (!acc) { Utils::printError("Account not found."); Utils::pauseForEnter(); return; }

        string pin = Utils::readValidatedPin("  PIN       : ");
        if (!acc->verifyPin(pin)) { Utils::printError("Incorrect PIN."); Utils::pauseForEnter(); return; }

        cout << "\n";
        acc->printHistory();
        Utils::pauseForEnter();
    }

    void transfer() {
        Utils::separator(C::BCYN, '=');
        Utils::printTitle("  FUND TRANSFER  ", C::BLD + C::BWHT, C::BG_CYN);
        Utils::separator(C::BCYN, '=');
        cout << "\n";

        string fromId = Utils::readValidatedAccountId("  From Account ID : ");
        Account* from = findAccount(fromId);
        if (!from) { Utils::printError("Source account not found."); Utils::pauseForEnter(); return; }

        string pin = Utils::readValidatedPin("  PIN              : ");
        if (!from->verifyPin(pin)) { Utils::printError("Incorrect PIN."); Utils::pauseForEnter(); return; }

        string toId = Utils::readValidatedAccountId("  To Account ID   : ");
        Account* to = findAccount(toId);
        if (!to)            { Utils::printError("Destination account not found."); Utils::pauseForEnter(); return; }
        if (fromId == toId) { Utils::printError("Cannot transfer to the same account."); Utils::pauseForEnter(); return; }

        Utils::printInfo("Available Balance", "INR " + Utils::fmtMoney(from->getBalance()),
                         C::BCYN, C::BWHT + C::BLD);
        double amount = Utils::readPositiveDouble("  Transfer Amount  : INR ");

        try {
            from->recordTransferOut(amount, toId);
            to->recordTransferIn(amount, fromId);
            saveData();
            log("Transfer INR " + Utils::fmtMoney(amount) + "  " + fromId + " -> " + toId);
            cout << "\n";
            Utils::printSuccess("Transfer successful!");
            Utils::printInfo(fromId + " balance", "INR " + Utils::fmtMoney(from->getBalance()),
                             C::BCYN, C::BYLW + C::BLD);
            Utils::printInfo(toId   + " balance", "INR " + Utils::fmtMoney(to->getBalance()),
                             C::BCYN, C::BGRN + C::BLD);
        } catch (const exception& e) { Utils::printError(e.what()); }
        Utils::pauseForEnter();
    }

    // ================================================================
    //  ADMIN PANEL
    // ================================================================

    void adminPanel() {
        string pin = Utils::readValidatedAdminPin("\n  Admin PIN: ");
        if (pin != ADMIN_PIN) {
            Utils::printError("Wrong admin PIN. Access denied.");
            Utils::pauseForEnter();
            return;
        }

        while (true) {
            cout << "\n";
            Utils::separator(C::BYLW, '=');
            Utils::printTitle("  ADMIN PANEL  ", C::BLD + C::BWHT, C::BG_YLW);
            Utils::separator(C::BYLW, '=');
            cout << "\n";
            menuItem(1, "View All Accounts",              C::BCYN);
            menuItem(2, "Search Account by Name",         C::BCYN);
            menuItem(3, "Delete Account  (moves to Trash)",C::BRED);
            menuItem(4, "View Trash / Recently Deleted",  C::BYLW);
            menuItem(5, "Restore Account from Trash",      C::BGRN);
            menuItem(6, "View System Log",                 C::BMAG);
            menuItem(0, "Back to Main Menu",               C::BRED);
            cout << "\n";

            int choice = Utils::readIntInRange("  Choice [0-6]: ", 0, 6);
            switch (choice) {
                case 0: return;
                case 1: adminListAll();          break;
                case 2: adminSearch();           break;
                case 3: adminDelete();           break;
                case 4: adminViewTrash();        break;
                case 5: adminRestoreFromTrash(); break;
                case 6: adminViewLog();          break;
            }
        }
    }

    void adminListAll() {
        cout << "\n";
        Utils::separator(C::BCYN);
        cout << "  " << C::BCYN << C::BLD
             << left  << setw(10) << "ID"
             << setw(24) << "Name"
             << setw(14) << "Type"
             << right << setw(14) << "Balance (INR)"
             << C::R << "\n";
        Utils::separator(C::DIM, '.');
        for (const auto* acc : accounts) {
            cout << "  " << C::BWHT  << left  << setw(10) << acc->getId()
                 << C::BWHT          << setw(24) << acc->getName()
                 << acc->typeColour()<< setw(14) << acc->accountType()
                 << C::BGRN          << right << setw(14)
                 << fixed << setprecision(2) << acc->getBalance()
                 << C::R << "\n";
        }
        Utils::separator(C::BCYN);
        cout << "  " << C::BYLW << "Total active accounts: "
             << C::BWHT << C::BLD << accounts.size() << C::R << "\n";
        Utils::pauseForEnter();
    }

    void adminSearch() {
        // readIntInRange already consumed its newline; buffer is clean here
        cout << "\n" << C::YLW << "  Search by name: " << C::R;
        string query; getline(cin, query);

        // Trim
        auto trim = [](string s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
            return s;
        };
        query = trim(query);

        if (query.empty()) {
            Utils::printError("Search query cannot be empty.");
            Utils::pauseForEnter();
            return;
        }

        string qLow = query;
        transform(qLow.begin(), qLow.end(), qLow.begin(), ::tolower);

        vector<Account*> results;
        for (auto* acc : accounts) {
            string nLow = acc->getName();
            transform(nLow.begin(), nLow.end(), nLow.begin(), ::tolower);
            if (nLow.find(qLow) != string::npos)
                results.push_back(acc);
        }

        if (results.empty()) {
            Utils::printError("No accounts found matching \"" + query + "\".");
        } else {
            cout << "\n" << C::BCYN << "  Found " << results.size()
                 << " result(s) for \"" << query << "\":\n" << C::R;
            for (auto* acc : results) { cout << "\n"; acc->printSummary(); }
        }
        Utils::pauseForEnter();
    }

    void adminDelete() {
        cout << "\n";
        string id = Utils::readValidatedAccountId("  Account ID to delete: ");
        auto it = find_if(accounts.begin(), accounts.end(),
                          [&](Account* a){ return a->getId() == id; });
        if (it == accounts.end()) {
            Utils::printError("Account not found.");
            Utils::pauseForEnter();
            return;
        }

        cout << "\n";
        Utils::printInfo("Account", (*it)->getId(),   C::BCYN, C::BWHT);
        Utils::printInfo("Holder",  (*it)->getName(), C::BCYN, C::BWHT);
        Utils::printInfo("Balance", "INR " + Utils::fmtMoney((*it)->getBalance()),
                         C::BCYN, C::BWHT);
        cout << "\n" << C::BYLW
             << "  This account will be moved to Trash and auto-deleted in 30 days.\n"
             << C::R;
        cout << C::BRED << C::BLD << "\n  Confirm delete? [y/N]: " << C::R;
        char confirm; cin >> confirm; Utils::clearInput();

        if (confirm != 'y' && confirm != 'Y') {
            cout << C::DIM << "  Cancelled.\n" << C::R;
            Utils::pauseForEnter();
            return;
        }

        string name = (*it)->getName();
        moveToTrash(*it);
        log("Account soft-deleted: " + id + " (" + name + ")");
        delete *it;
        accounts.erase(it);
        saveData();
        Utils::printSuccess("Account moved to Trash. Permanently deleted in 30 days.");
        Utils::pauseForEnter();
    }

    void adminViewTrash() {
        // Purge expired entries
        trash.erase(remove_if(trash.begin(), trash.end(),
                              [](const TrashedEntry& e){ return e.isExpired(); }),
                    trash.end());
        saveTrash();

        cout << "\n";
        Utils::separator(C::BRED, '=');
        Utils::printTitle("  TRASH BIN  (30-Day Retention)  ", C::BLD + C::BWHT, C::BG_RED);
        Utils::separator(C::BRED, '=');

        if (trash.empty()) {
            cout << C::DIM << "\n  Trash is empty.\n" << C::R;
            Utils::pauseForEnter();
            return;
        }

        cout << "  " << C::BRED << C::BLD
             << left  << setw(10) << "ID"
             << setw(22) << "Name"
             << setw(14) << "Type"
             << setw(22) << "Deleted On"
             << right << setw(10) << "Days Left"
             << C::R << "\n";
        Utils::separator(C::DIM, '.');

        for (const auto& e : trash) {
            int dl = e.daysLeft();
            string dlCol = (dl <= 7) ? C::BRED : C::BYLW;
            cout << "  " << C::BWHT << left  << setw(10) << e.accountId
                 << setw(22) << e.holderName
                 << C::DIM   << setw(14) << e.accType
                 << C::BWHT  << setw(22) << e.deletedAt
                 << dlCol << right << setw(7) << to_string(dl) + " day(s)"
                 << C::R << "\n";
        }
        Utils::separator(C::BRED);
        cout << "  " << C::BYLW << "Entries in trash: "
             << C::BWHT << C::BLD << trash.size() << C::R
             << C::DIM << "  (entries <= 7 days left shown in red)\n" << C::R;
        Utils::pauseForEnter();
    }

    void adminRestoreFromTrash() {
        // Purge expired first
        trash.erase(remove_if(trash.begin(), trash.end(),
                              [](const TrashedEntry& e){ return e.isExpired(); }),
                    trash.end());
        saveTrash();

        cout << "\n";
        Utils::separator(C::BYLW, '=');
        Utils::printTitle("  RESTORE FROM TRASH  ", C::BLD + C::BWHT, C::BG_YLW);
        Utils::separator(C::BYLW, '=');

        if (trash.empty()) {
            cout << C::DIM << "\n  Trash is empty. Nothing to restore.\n" << C::R;
            Utils::pauseForEnter();
            return;
        }

        // List trash entries with index
        cout << "\n  " << C::BCYN << C::BLD
             << left  << setw(5)  << "#"
             << setw(10) << "ID"
             << setw(22) << "Name"
             << setw(14) << "Type"
             << right << setw(10) << "Days Left"
             << C::R << "\n";
        Utils::separator(C::DIM, '.');
        for (int i = 0; i < (int)trash.size(); ++i) {
            const auto& e = trash[i];
            int dl = e.daysLeft();
            string dlCol = (dl <= 7) ? C::BRED : C::BYLW;
            cout << "  " << C::BCYN << C::BLD << left << setw(5) << (i + 1) << C::R
                 << C::BWHT << setw(10) << e.accountId
                 << setw(22) << e.holderName
                 << C::DIM  << setw(14) << e.accType
                 << dlCol << right << setw(7) << to_string(dl) + "d"
                 << C::R << "\n";
        }
        Utils::separator(C::BYLW, '.');

        int sel = Utils::readIntInRange(
            "  Enter 1 to restore (0 to cancel): ", 0, (int)trash.size());
        if (sel == 0) {
            cout << C::DIM << "  Cancelled.\n" << C::R;
            Utils::pauseForEnter();
            return;
        }

        TrashedEntry& entry = trash[sel - 1];

        // Check no ID clash with existing active accounts
        if (findAccount(entry.accountId) != nullptr) {
            Utils::printError("An active account with ID " + entry.accountId +
                              " already exists. Cannot restore.");
            Utils::pauseForEnter();
            return;
        }

        // Rebuild Account object from TrashedEntry
        Account* acc = nullptr;
        if      (entry.accType == "Savings")      acc = new SavingsAccount();
        else if (entry.accType == "Current")      acc = new CurrentAccount();
        else if (entry.accType == "FixedDeposit") acc = new FixedDepositAccount();
        else                                      acc = new SavingsAccount();

        acc->restoreFields(entry.accountId, entry.holderName,
                           entry.pin, entry.balance, entry.createdAt);

        // Replay history via temp file
        {
            ofstream tmp("__restore_tmp__.dat", ios::trunc);
            tmp << "ACCOUNT|" << entry.accType << "|" << entry.accountId << "|"
                << entry.holderName << "|" << entry.pin << "|"
                << fixed << setprecision(6) << entry.balance << "|"
                << entry.createdAt << "\n";
            tmp << "TXCOUNT|" << entry.history.size() << "\n";
            for (const auto& tx : entry.history)
                tmp << "TX|" << tx.serialise() << "\n";
            tmp << "END\n";
            tmp.close();

            ifstream rin("__restore_tmp__.dat");
            string line;
            getline(rin, line); // ACCOUNT header
            size_t cnt = 0;
            if (getline(rin, line) && line.rfind("TXCOUNT|", 0) == 0)
                cnt = stoul(line.substr(8));
            acc->loadHistory(rin, cnt);
            rin.close();
            remove("__restore_tmp__.dat");
        }

        accounts.push_back(acc);
        saveData();

        string restoredId   = entry.accountId;
        string restoredName = entry.holderName;
        trash.erase(trash.begin() + (sel - 1));
        saveTrash();
        log("Account restored from trash: " + restoredId + " (" + restoredName + ")");

        cout << "\n";
        Utils::printSuccess("Account " + restoredId + " (" + restoredName +
                            ") has been restored successfully!");
        Utils::pauseForEnter();
    }

    void adminViewLog() {
        ifstream lf(LOG_FILE);
        if (!lf.is_open()) {
            Utils::printError("No log file found.");
            Utils::pauseForEnter();
            return;
        }
        cout << "\n";
        Utils::separator(C::BMAG, '=');
        Utils::printTitle("  SYSTEM ACTIVITY LOG (last 25 entries)  ",
                          C::BLD + C::BWHT, C::BG_MAG);
        Utils::separator(C::BMAG, '=');
        vector<string> lines;
        string line;
        while (getline(lf, line)) lines.push_back(line);
        int start = max(0, (int)lines.size() - 25);
        for (int i = start; i < (int)lines.size(); ++i)
            cout << "  " << C::DIM << lines[i] << C::R << "\n";
        Utils::separator(C::BMAG, '=');
        Utils::pauseForEnter();
    }

public:
    Bank()  { loadData(); loadTrash(); }
    ~Bank() { saveData(); saveTrash(); for (auto* a : accounts) delete a; }

    void run() {
        // Welcome banner
        cout << "\n"
             << C::BG_BLU << C::BLD << C::BWHT
             << "  ================================================================  \n"
             << "         BANK MANAGEMENT SYSTEM                                     \n"
             << "  ================================================================  \n"
             << C::R << "\n";

        while (true) {
            Utils::separator(C::BBLU, '=');
            Utils::printTitle("  MAIN MENU  ", C::BLD + C::BWHT, C::BG_BLU);
            Utils::separator(C::BBLU, '=');
            cout << "\n";
            menuItem(1, "Create New Account", C::BGRN);
            menuItem(2, "Deposit Funds",      C::BGRN);
            menuItem(3, "Withdraw Funds",     C::BRED);
            menuItem(4, "Balance Inquiry",    C::BBLU);
            menuItem(5, "Fund Transfer",      C::BCYN);
            menuItem(6, "Transaction History",C::BMAG);
            menuItem(7, "Admin Panel",        C::BYLW);
            menuItem(0, "Exit",               C::BRED);
            cout << "\n";

            int choice = Utils::readIntInRange("  Select option [0-7]: ", 0, 7);
            cout << "\n";

            switch (choice) {
                case 1: createAccount(); break;
                case 2: deposit();       break;
                case 3: withdraw();      break;
                case 4: checkBalance();  break;
                case 5: transfer();      break;
                case 6: viewHistory();   break;
                case 7: adminPanel();    break;
                case 0:
                    cout << "\n";
                    Utils::separator(C::BGRN, '=');
                    Utils::printTitle("  Thank you for banking with us!  ",
                                      C::BLD + C::BWHT, C::BG_GRN);
                    Utils::separator(C::BGRN, '=');
                    cout << "\n";
                    return;
            }
        }
    }
};

// ================================================================
//  ENTRY POINT
// ================================================================
int main() {
    Bank bank;
    bank.run();
    return 0;
}

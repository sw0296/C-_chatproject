//ä�� ����
#pragma comment(lib, "ws2_32.lib") //������� ���̺귯���� ��ũ. ���� ���̺귯�� ����
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h> // C++�ϰ� MYSQL�� �����ϱ� ���� ����.
#include <sstream> // �䱸���� �и��ؼ� �����ϱ� ���� �ʿ���.
#include <WS2tcpip.h> // ���� ������ ���� ���
#include <mutex>

#define MAX_SIZE 1024
#define MAX_CLIENT 5

using std::cout;
using std::cin;
using std::endl;
using std::string;

// MY SQL ���� ������ �����ϴ� �����ϴ� �� //
const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "root"; // �����ͺ��̽� �����
const string password = "ek970113"; // �����ͺ��̽� ���� ��й�ȣ

// MySQL Connector/C++ �ʱ�ȭ
sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* res; //������� ����


SOCKET client_sock;

struct SOCKET_INFO { // ����� ���� ������ ���� Ʋ ����
    SOCKET sck;
    string user;
    int user_number;
    bool login_status = false;
    int ti;
};

std::vector<SOCKET_INFO> sck_list; // ����� Ŭ���̾�Ʈ ���ϵ��� ������ �迭 ����.
SOCKET_INFO server_sock; // ���� ���Ͽ� ���� ������ ������ ���� ����.

std::vector<std::string> tokens; // ���Ͽ� ���� ������ ��� ���� ��ū �迭
std::string token; //��ū �迭
std::mutex mtx;

int client_count = 0; // ���� ������ �ִ� Ŭ���̾�Ʈ�� count �� ���� ����.
string test_count;
string chatroom_num;

bool login_result = false;
bool join_result = false;
bool join_check = false;
bool user_check = false;
void server_init(); // socket �ʱ�ȭ �Լ�. socket(), bind(), listen() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client(); // ���Ͽ� ������ �õ��ϴ� client�� �߰�(accept)�ϴ� �Լ�. client accept() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void send_msg(const char* msg); // send() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void recv_msg(int idx); // recv() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
//void dm_send_msg(const string& sender, const char* msg, const string& recipientUser);
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid);
void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue);
void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser);
//int chat_recv();
void db_total_msg();

SOCKET getSocket(string user);
//int removeSocket(string user);
// ��� ����
void del_client(int idx); // ���Ͽ� ����Ǿ� �ִ� client�� �����ϴ� �Լ�. closesocket() �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
// MYSQL DB ���� ������ �Լ� ������ //
// ���߿� �� �ο��� ���� �� �Լ� �߰��� �ʿ伺 ����!! init �� delete�� ��� ���� �ʼ�. //
void db_init(); // db ���̽� ���� �� �ѱ� ���� ���� �κ� ����
//void db_messageSend(); // �޽��� ���� ����
void db_join(); //ȸ������
void db_join_check(); // ȸ������ �� ���̵� üũ
void db_UserEdit(); // ȸ�� ���� ������ �� ��й�ȣ Ȯ�ι�
void db_UserEdit_update(); // ȸ�� ���� ���� ������Ʈ��
void db_login();
void db_findID(); // (7) ���� ���� ã��
void db_chat(int idx, string msg);

void send_pastchat(const char* msg, const string& recipientUser);

void db_init() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }
    // �����ͺ��̽� ����
    con->setSchema("chatproject");
    // db �ѱ� ������ ���� ����
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }
}

void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Internet�� Stream ������� ���� ����
   // SOCKET_INFO�� ���� ��ü�� socket �Լ� ��ȯ��(��ũ���� ����)
   // ���ͳ� �ּ�ü��, ��������, TCP �������� �� ��.
    SOCKADDR_IN server_addr = {}; // ���� �ּ� ���� ����
    // ���ͳ� ���� �ּ�ü�� server_addr
    server_addr.sin_family = AF_INET; // ������ Internet Ÿ��
    server_addr.sin_port = htons(7777); // ���� ��Ʈ ����
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // �����̱� ������ local �����Ѵ�.
    //Any�� ���� ȣ��Ʈ�� 127.0.0.1�� ��Ƶ� �ǰ� localhost�� ��Ƶ� �ǰ� ���� �� ����ϰ� �� �� �ֵ�. �װ��� INADDR_ANY�̴�.
    //ip �ּҸ� ������ �� server_addr.sin_addr.s_addr -- ������ ���?
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // ������ ���� ������ ���Ͽ� ���ε��Ѵ�.
    listen(server_sock.sck, SOMAXCONN); // ������ ��� ���·� ��ٸ���.
    server_sock.user = "server";
    cout << "Server On" << endl;
}

//�α���
void db_login() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    if (res->next()) {
        string db_id = res->getString("user_id"); // �����ͺ��̽��� id ����
        string db_pw = res->getString("pw"); // �����ͺ��̽��� ��й�ȣ ����
        string db_name = res->getString("name"); // �����ͺ��̽��� �̸� ����

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_id == tokens[1] && db_pw == tokens[2]) {
            login_result = true;
            int result = 1;
            int server_request = 1;
            int str_test_count = stoi(test_count);
            sck_list[str_test_count].user = db_name;
            dm_send_result(server_request, "server", result, test_count, db_name, db_id);
        }
        else if (db_id != tokens[1] || db_pw != tokens[2]) {
            int result = 0;
            int server_request = 1;
            dm_send_result(server_request, "server", result, test_count, "�ӽ�����", "temp");
        }
    }
    else {
        int result = 2;
        int server_request = 1;
        dm_send_result(server_request, "server", result, test_count, "�ӽ�����", "temp");
    }
    return;
}
//ȸ������
void db_join() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber, nickname FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    if (res->next()) {
        join_result = false;
        int result = 3;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[2], "temp");
    }
    else {

        stmt = con->createStatement();
        pstmt = con->prepareStatement("INSERT INTO users (user_id, name, pw, phonenumber, nickname) values(?,?,?,?,?)"); // INSERT

        pstmt->setString(1, tokens[1]); //���̵�
        pstmt->setString(2, tokens[2]); // �̸�
        pstmt->setString(3, tokens[3]); // ��й�ȣ
        pstmt->setString(4, tokens[4]); // ��ȭ��ȣ
        pstmt->setString(5, tokens[5]); // �г���

        pstmt->execute(); // �̰� �־���� ��� �����.

        join_result = true;
        int result = 1;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        sck_list[str_test_count].user = tokens[1];
        dm_send_result(server_request, "server", result, test_count, tokens[2], "temp");
        cout << "�ű� ���� ������ �Ϸ�Ǿ����ϴ�." << endl;
    }
}
//db���̵� üũ 
void db_join_check() {

    pstmt = con->prepareStatement("SELECT user_id FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    if (res->next()) {
        join_check = false;
        user_check = true;
        int result = 4; // ���̵� üũ ����
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1], "temp");
    }

    else {
        join_check = true;
        user_check = false;
        int result = 3; // ���̵� üũ ���� �����
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1], "temp");
    }
}
//db PWã��
void db_findPW() {
    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_pw = res->getString("pw");
        string db_phonenumber = res->getString("phonenumber");

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_id == tokens[1] && db_name == tokens[2] && db_phonenumber == tokens[3]) {
            int result = 1; // ���� �� �����
            int server_request = 3; // PW ã�� ��ȣ
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_pw); // findValue = user_id
        }
        else {
            int result = 2; // ���� �� �����
            int server_request = 3; // PW ã�� ��ȣ
            string fail = "����";
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // ���� �� �����
        int server_request = 2; // ID ã�� ��ȣ
        string fail = "����";
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
    }
}
//db IDã��
void db_findID() {
    pstmt = con->prepareStatement("SELECT user_id, name, phonenumber FROM users WHERE phonenumber = ?");
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_phonenumber = res->getString("phonenumber");

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_name == tokens[1] && db_phonenumber == tokens[2]) {
            int result = 1; // ���� �� �����
            int server_request = 2; // ID ã�� ��ȣ
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_id); // findValue = user_id
        }
        else {
            int result = 2; // ���� �� �����
            int server_request = 2; // ID ã�� ��ȣ
            string fail = "����";
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // ���� �� �����
        int server_request = 2; // ID ã�� ��ȣ
        string fail = "����";
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
    }
}
//db���� PWã�Ƽ� ����
void db_UserEdit() {
    // �����ͺ��̽����� ���� ��й�ȣ�� �������� ����
    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
    pstmt = con->prepareStatement(selectQuery);
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    if (res->next()) {
        string database_password = res->getString("pw");

        // ����ڰ� �Է��� ��й�ȣ�� �����ͺ��̽��� ��й�ȣ ��
        if (tokens[1] == database_password) {
            // �Է��� ��й�ȣ�� �����ͺ��̽� ��й�ȣ�� ��ġ�ϸ� ������Ʈ ����
            cout << "Ȯ�� �Ǿ����ϴ�." << endl;
            int result = 1;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
        }
        else {
            cout << "�Է��� ��й�ȣ�� ��ġ���� �ʽ��ϴ�." << endl;
            int result = 2;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
        }
    }
    else {
        cout << "���� : ������� �α��� ������ Ȯ���� �� �����ϴ�. (����� ���̵� ����)" << endl;
    }
}
//db PW ����
void db_UserEdit_update() {

    string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
    pstmt = con->prepareStatement(updateQuery);
    pstmt->setString(1, tokens[1]); //��ū��1�� �ٲܺ�й�ȣ
    pstmt->setString(2, tokens[2]); //��ū��2�� �������̵�
    pstmt->executeUpdate();

    cout << tokens[2] << " �� ��й�ȣ�� ���� �Ǿ����ϴ�." << endl;
    int result = 3;
    int str_test_count = stoi(test_count);
    dm_send_resultEdit(8, "server", result, test_count);

}
//�г��� �̿ϼ�
void db_UserEditnick_update() {
    string updateQuery = "UPDATE users SET nickname = ? WHERE user_id = ?";
    pstmt = con->prepareStatement(updateQuery);
    pstmt->setString(1, tokens[1]); //��ū��1�� �ٲܺ�й�ȣ
    pstmt->setString(2, tokens[2]); //��ū��2�� �������̵�
    pstmt->executeUpdate();

    cout << "�г����� ����Ǿ����ϴ�." << endl;
    int result = 3;
    int str_test_count = stoi(test_count);
    dm_send_resultEdit(9, "server", result, test_count);
}
//db�� ä�ó��� �ֱ�
void db_chat(int idx, string msg) {
    db_init();
    pstmt = con->prepareStatement("INSERT INTO chatting(name,contents) VALUES(?,?)");
    pstmt->setString(1, sck_list[idx].user + "  : ");
    pstmt->setString(2, msg);
    pstmt->execute();


}
//db���� ���� ä�ó��� �ҷ�����
void db_total_msg() {

    db_init();
    pstmt = con->prepareStatement("SELECT * FROM chatting");

    res = pstmt->executeQuery();

    while (res->next()) {
        string text;
        int str_test_count = stoi(test_count);
        /*std::cout << "ID: " << res->getString("name") << ", User: " << res->getString("name") << ", Message: " << res->getString("contents") << std::endl;*/
        text = res->getString("name") + res->getString("contents") + "\n";
        //cout << text << endl;         //������ ���� �޼����� �߰� �ϰ� ������


        send_pastchat(text.c_str(), test_count);

    }


}

// Ư�� ����ڿ��� �޼���
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + username + " " + userid;
    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
        }
    }
    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}

void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser;
    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
        }
    }
    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}
//���� ä�ó��� ����(�����ϴ� Ŭ���̾�Ʈ���Ը�)
void send_pastchat(const char* msg, const string& recipientUser) {
    string result = msg;

    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
        }
    }
}
//ID,PWã�� ��� ����
void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + findValue;
    for (int i = 0; i < client_count; i++) {

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
        }
    }

    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}

int main() {
    WSADATA wsa;
    system("title ����");
    // Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
    // ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
    // 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        server_init();
        std::thread th1[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) {
            // �ο� �� ��ŭ thread �����ؼ� ������ Ŭ���̾�Ʈ�� ���ÿ� ������ �� �ֵ��� ��.
            th1[i] = std::thread(add_client);
        }
        //recv_msg(sck_list[client_count].user_number);
        //std::thread th1(add_client); // �̷��� �ϸ� �ϳ��� client�� �޾���...
        while (1) { // ���� �ݺ����� ����Ͽ� ������ ����ؼ� ä�� ���� �� �ִ� ���¸� ����� ��. �ݺ����� ������� ������ �� ���� ���� �� ����.            
            string text, msg = "";
            std::getline(cin, text);
            const char* buf = text.c_str();
            //msg = buf;
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
        }
        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
            //join : �ش��ϴ� thread ���� ������ �����ϸ� �����ϴ� �Լ�.
            //join �Լ��� ������ main �Լ��� ���� ����Ǿ thread�� �Ҹ��ϰ� ��.
            //thread �۾��� ���� ������ main �Լ��� ������ �ʵ��� ����.
        }
        closesocket(server_sock.sck);
    }
    else {
        cout << "���α׷� ����. (Error code : " << code << ")";
    }

    WSACleanup();
    return 0;

}

void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };
    ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

    SOCKET_INFO new_client = {};
    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    // Winsock2�� recv �Լ�. client�� ���� �г����� ����.
    //new_client.user = string(buf);
    cout << "buf = > " << buf << endl;

    sck_list.push_back(new_client); // client ������ ��� sck_list �迭�� ���ο� client �߰�        
    std::thread th(recv_msg, client_count);

    sck_list[client_count].login_status = false;
    sck_list[client_count].user_number = client_count;
    cout << "========================" << endl;
    cout << "���ο� ������ �����߽��ϴ�." << endl;
    cout << "sck_list[client_count].login_status = " << sck_list[client_count].login_status << endl;
    cout << "sck_list[client_count].user_number = " << sck_list[client_count].user_number << endl;
    cout << "========================" << endl;
    client_count++; // client �� ���� 
    th.join();
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}

void recv_msg(int idx) {
    //char buf[MAX_SIZE] = { };
    string msg = "";


    //cout << sck_list[idx].user << endl;
    while (1) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // ������ �߻����� ������ recv�� ���ŵ� ����Ʈ ���� ��ȯ. 0���� ũ�ٴ� ���� �޽����� �Դٴ� ��.
            cout << "========================" << endl;
            //mtx.lock();
            msg = buf; //sck_list[idx].user
            //send_msg(msg.c_str());
            std::istringstream iss(buf);
            tokens.clear(); // ���� ��ū�� ����� ���� ���۾��ϸ� �� �����Ǽ� ����� �ν� ���� �ФФФФ�
            test_count = std::to_string(sck_list[idx].user_number);
            while (iss >> token) {
                tokens.push_back(token);
            }
            // ��ū0 �������� 1:�α��� / 2:idã�� / 3:pwã�� / 4:ȸ������ / 5:��ȭ / 6:����ä�� / 7:ģ�� / 8:�������
            // tokens[0] == 1 �̸� �α��� ��û
            if (tokens[0] == "1") {
                cout << tokens[1] << "�� ���̵� ������ �α��� ��û�� ���Խ��ϴ�." << endl;
                db_init();
                db_login();
            };
            // tokens[0] == 2 �̸� ���̵� ã�� ��û
            if (tokens[0] == "2") {
                cout << tokens[1] << " ȸ���� ���̵� ã�� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_findID();
            };
            // tokens[0] == 3 �̸� ��й�ȣ ã�� ��û
            if (tokens[0] == "3") {
                cout << tokens[1] << " ȸ���� ��й�ȣ ã�� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_findPW();
            };
            // tokens[0] == 4 �̸� ȸ������ ��û
            if (tokens[0] == "4") {
                cout << " ȸ������ ��û�� ���Խ��ϴ�." << endl;
                db_init();
                db_join();
            };
            if (tokens[0] == "41") {
                cout << " ���̵� Ȯ�� ��û�� ���Խ��ϴ�." << endl;
                db_init();
                db_join_check();
            };
            //tokens[0] == 51 �̸� ���� �޽��� ���� ��û
            if (tokens[0] == "5") {  // ���� �޼����� ��������
                db_total_msg();
            }

            if (tokens[0] == "51") {

                if (tokens[1].empty() == false) {
                    size_t pos = msg.find("51");
                    msg.erase(pos, 3);

                    db_chat(idx, msg);

                    msg = sck_list[idx].user + " : " + msg;
                    cout << msg << endl;

                    send_msg(msg.c_str());

                }


            }


            // tokens[0] == 8 �̸� ��й�ȣ ���� ��û
            if (tokens[0] == "8") {
                if (tokens[3] == "N") { //tokens[0] �� 8 �̸鼭 tokens[3] �� ���� Y�� N���� ��й�ȣ Ȯ�� ����� �����մϴ�.
                    cout << tokens[1] << " [��й�ȣ Ȯ�� ��û] ��ū[1]�� ��й�ȣ������ �������� ��й�ȣ ���� ��û�� ���Խ��ϴ�." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit(); // �����ͺ��̽� ���� ����
                    tokens.clear(); // ���� ��ū�� ����� ���� ���۾��ϸ� �� �����Ǽ� ����� �ν� ���� �ФФФФ�
                }
                if (tokens[3] == "Y") {
                    cout << tokens[1] << " [��й�ȣ Ȯ�� �Ϸ�] ��ū[1]�� ��й�ȣ������ �������� ��й�ȣ ���� ��û�� ���Խ��ϴ�." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit_update();
                    tokens.clear(); // ���� ��ū�� ����� ���� ���۾��ϸ� �� �����Ǽ� ����� �ν� ���� �ФФФФ�
                }
            }



        }

        else { //�׷��� ���� ��� ���忡 ���� ��ȣ�� �����Ͽ� ���� �޽��� ����
            msg = "[����] " + sck_list[idx].user + " ���� �����߽��ϴ�.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx); // Ŭ���̾�Ʈ ����
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    client_count--;
}













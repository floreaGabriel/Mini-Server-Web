#include "PostHandler.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>

PostHandler::PostHandler() {
}

PostHandler::~PostHandler() {
}

//functie pentru a extrage payload-ul din buffer
//este la finalul bufferului delimitat de un enter inainte de a incepe acesta
std::string PostHandler::extractPayLoad(const char* buffer) {
    //cauta liniile goale pentru a gasi de unde incepe payload-ul
    const char* payload_start = strstr(buffer, "\r\n\r\n");
    if (!payload_start) {
        payload_start = strstr(buffer, "\n\n");
    }

    if (payload_start) {
        //muta la inceputul payload-ului pointerul
        payload_start += (payload_start[1] == '\n') ? 2 : 4;
        return std::string(payload_start);
    }

    return ""; // Returnăm un string gol dacă nu găsim delimitatorul
}

bool PostHandler::usernameExists(const std::string& filename, const std::string& username) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nu s-a putut deschide fișierul " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t first_space = line.find(' '); //caut primul spatiu in linie pentru a-mi da seama unde este primul cuvant
        std::string current_username = (first_space != std::string::npos) 
                                       ? line.substr(0, first_space) //extrag username-ul
                                       : line; //daca nu este spatiu, toata linia este doar username
        
        if (current_username == username) {
            return true; // Am găsit username-ul
        }
    }

    return false; // Username-ul nu a fost găsit
}

std::string PostHandler::getPasswordForUsername(const std::string& filename, const std::string& username) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nu s-a putut deschide fișierul " << filename << std::endl;
        return ""; 
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t first_space = line.find(' '); //caut primul space
        std::string current_username = (first_space != std::string::npos) 
                                       ? line.substr(0, first_space)  // extrag username-ul
                                       : line; 
        if (current_username == username) {
            return line.substr(first_space + 1); // daca username-ul este acelasi, dau return la parola
        }
    }

    return ""; // Returnăm un string gol dacă username-ul nu este găsit
}

void PostHandler::handlePostRequest(const std::string& path, int client_socket, const char* buffer) {
    std::string payload = extractPayLoad(buffer);

    if (path == "/register") {
        handleRegister(client_socket, payload);
    } else if (path == "/signin") {
        handleSignIn(client_socket, payload);
    }
}

void PostHandler::handleRegister(int client_socket, const std::string& payload) {
    std::string username, password;
    parseCredentials(payload, username, password);

    if (usernameExists("resources/accounts.txt", username)) {
        // Utilizatorul există deja
        const char* response = "HTTP/1.1 409 Conflict\r\nContent-Type: text/plain\r\n\r\nUsername already exists!";
        send(client_socket, response, strlen(response), 0);
    } else {
        //utilizatorul nu exista ->
        // Adăugăm utilizatorul
        std::ofstream file("resources/accounts.txt", std::ios::app);
        if (!file.is_open()) {
            const char* response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nCould not open accounts file!";
            send(client_socket, response, strlen(response), 0);
            return;
        }

        file << username << " " << password << std::endl;
        const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRegistration successful!";
        send(client_socket, response, strlen(response), 0);
    }
}

void PostHandler::handleSignIn(int client_socket, const std::string& payload) {
    std::string username, password;
    parseCredentials(payload, username, password);

    if (usernameExists("resources/accounts.txt", username)) {
        //username-ul exista in fisier -> 
        std::string storedPassword = getPasswordForUsername("resources/accounts.txt", username);
        if (storedPassword == password) {
            //parola si username-ul coincid -> dau login cu succes
            const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nLogin successful!";
            send(client_socket, response, strlen(response), 0);
        } else {
            //degeaba exista username-ul daca nu se potriveste cu parola
            const char* response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\n\r\nIncorrect password!";
            send(client_socket, response, strlen(response), 0);
        }
    } else {
        //nu exista acel username in baza noastra de date
        const char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nUsername does not exist!";
        send(client_socket, response, strlen(response), 0);
    }
}

void PostHandler::parseCredentials(const std::string& payload, std::string& username, std::string& password) {
    // Extrage username-ul și parola din payload
    size_t username_pos = payload.find("username=");
    if (username_pos != std::string::npos) {
        username_pos += 9;  // Sărim peste "username="
        size_t username_end = payload.find("&", username_pos);
        username = (username_end != std::string::npos) 
                   ? payload.substr(username_pos, username_end - username_pos) 
                   : payload.substr(username_pos);
    }

    size_t password_pos = payload.find("password=");
    if (password_pos != std::string::npos) {
        password_pos += 9;  // Sărim peste "password="
        password = payload.substr(password_pos);
    }
}

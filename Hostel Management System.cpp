#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime> // Include the ctime header for timestamp

struct Student {
    int enrollmentNumber;
    std::string name;
    int yearOfStudy;
    std::string mobileNumber;
    std::string email;
    std::string guardianName;
    std::string guardianMobileNumber;
    std::string homeAddress;
    double feesDue;
    std::string roomPreference;
};

std::vector<Student> students;
std::vector<bool> roomStatus(20, false);
// Function declarations
void loadStudentDataFromFile();
void saveStudentDataToFile();
bool isFeesPaid(int enrollmentNumber);
void payFees(int enrollmentNumber);
void bookRoom(int enrollmentNumber);
void registerStudent();
int loginStudent();
void viewStudentInfo();
void updateStudentInfo();
void deleteStudentInfo();
void addStudent();
void adminMenu();
void studentMenu(int enrollmentNumber);
void helpDesk();
void searchStudentByEnrollment();
void viewRoomStatus();

int main() {
    loadStudentDataFromFile(); // Load existing student data from file

    int userType;
    while (true) {
        std::cout << "\nChoose User Type:" << std::endl;
        std::cout << "1. Admin" << std::endl;
        std::cout << "2. Student" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> userType;

        switch (userType) {
            case 1:
                adminMenu();
                break;
            case 2: {
                int studentChoice;
                std::cout << "1. Register\n2. Login\nEnter your choice: ";
                std::cin >> studentChoice;

                switch (studentChoice) {
                    case 1:
                        registerStudent();
                        break;
                    case 2: {
                        int enrollmentNumber = loginStudent();
                        if (enrollmentNumber != -1) {
                            studentMenu(enrollmentNumber);
                        } else {
                            std::cout << "Login failed. Enrollment number not found." << std::endl;
                        }
                        break;
                    }
                    default:
                        std::cout << "Invalid choice. Please try again." << std::endl;
                }
                break;
            }
            case 3:
                std::cout << "Exiting the program." << std::endl;
                saveStudentDataToFile(); // Save student data to file before exiting
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void loadStudentDataFromFile() {
    std::ifstream inFile("students.txt");

    if (inFile.is_open()) {
        students.clear();

        while (!inFile.eof()) {
            Student student;
            inFile >> student.enrollmentNumber >> student.name >> student.roomPreference >> student.feesDue;
            students.push_back(student);
        }

        inFile.close();
    }
}

void saveStudentDataToFile() {
    std::ofstream outFile("students.txt");

    if (outFile.is_open()) {
        for (const Student& student : students) {
            outFile << student.enrollmentNumber << " " << student.name << " " << student.roomPreference << " " << student.feesDue << "\n";
        }

        outFile.close();
    }
}

bool isFeesPaid(int enrollmentNumber) {
    for (const Student& student : students) {
        if (student.enrollmentNumber == enrollmentNumber) {
            return student.feesDue == 0.0;
        }
    }

    return false;
}

void payFees(int enrollmentNumber) {
    for (Student& student : students) {
        if (student.enrollmentNumber == enrollmentNumber) {
            if (student.feesDue == 0.0) {
                std::cout << "Fees already paid." << std::endl;
            } else {
                std::cout << "Your dues: " << student.feesDue << std::endl;

                double amount;
                std::cout << "Enter the amount to pay (or 0 to cancel): ";
                std::cin >> amount;

                if (amount == 0.0) {
                    std::cout << "Payment canceled." << std::endl;
                } else if (amount >= student.feesDue) {
                    std::cout << "Fees paid successfully. Change: " << (amount - student.feesDue) << std::endl;
                    student.feesDue = 0.0;
                } else {
                    student.feesDue -= amount;
                    std::cout << "Partial payment successful. Remaining dues: " << student.feesDue << std::endl;
                }

                // Generate and display the fee receipt
                time_t now = time(0);
                tm* localTime = localtime(&now);

                std::cout << "\nFee Receipt:" << std::endl;
                std::cout << "Enrollment Number: " << student.enrollmentNumber << std::endl;
                std::cout << "Name: " << student.name << std::endl;
                std::cout << "Year of Study: " << student.yearOfStudy << std::endl;
                std::cout << "Mobile Number: " << student.mobileNumber << std::endl;
                std::cout << "Email: " << student.email << std::endl;
                std::cout << "Guardian's Name: " << student.guardianName << std::endl;
                std::cout << "Guardian's Mobile Number: " << student.guardianMobileNumber << std::endl;
                std::cout << "Home Address: " << student.homeAddress << std::endl;
                std::cout << "Room Preference: " << student.roomPreference << std::endl;
                std::cout << "Fees Paid: " << (1000.0 - student.feesDue) << std::endl;  // Corrected this line
                std::cout << "Remaining Dues: " << student.feesDue << std::endl;
                std::cout << "Payment Date: " << asctime(localTime);
            }

            return;
        }
    }

    std::cout << "Student not found with enrollment number " << enrollmentNumber << std::endl;
}
void bookRoom(int enrollmentNumber) {
    for (Student& student : students) {
        if (student.enrollmentNumber == enrollmentNumber) {
            if (!isFeesPaid(enrollmentNumber)) {
                std::cout << "Please pay your fees first to book a room." << std::endl;
                return;
            }

            if (student.feesDue > 0.0) {
                std::cout << "You have dues of " << student.feesDue << ". Please pay your dues first." << std::endl;
                payFees(enrollmentNumber);
            }

            if (student.roomPreference.empty()) {
                std::string roomPreference;
                std::cout << "Enter your room preference (Single/Double/Triple): ";
                std::cin >> roomPreference;

                if (roomPreference == "Single") {
                    // For Single sharing, ask only for the student's details
                    std::cout << "Enter your name: ";
                    std::cin.ignore(); // Ignore the newline character in the buffer
                    std::getline(std::cin, student.name);

                    std::cout << "Enter your enrollment number: ";
                    std::cin >> student.enrollmentNumber;

                    std::cout << "Enter your year of study: ";
                    std::cin >> student.yearOfStudy;
                } else if (roomPreference == "Double") {
                    // For Double sharing, ask for details of two students
                    std::cout << "Enter details for Student 1:" << std::endl;
                    std::cout << "Name: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character
                    std::getline(std::cin, student.name);

                    std::cout << "Enrollment number: ";
                    std::cin >> student.enrollmentNumber;

                    std::cout << "Year of study: ";
                    std::cin >> student.yearOfStudy;

                    // Create Student 2
                    Student student2;
                    std::cout << "Enter details for Student 2:" << std::endl;
                    std::cout << "Name: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character
                    std::getline(std::cin, student2.name);

                    std::cout << "Enrollment number: ";
                    std::cin >> student2.enrollmentNumber;

                    std::cout << "Year of study: ";
                    std::cin >> student2.yearOfStudy;

                    // Continue with the common room booking logic
                    std::cout << "Room booked successfully." << std::endl;

                    // Generate receipt
                    time_t now = time(0);
                    tm* localTime = localtime(&now);

                    std::cout << "\nReceipt:" << std::endl;
                    std::cout << "Enrollment Number (Student 1): " << student.enrollmentNumber << std::endl;
                    std::cout << "Name (Student 1): " << student.name << std::endl;
                    std::cout << "Year of Study (Student 1): " << student.yearOfStudy << std::endl;

                    std::cout << "Enrollment Number (Student 2): " << student2.enrollmentNumber << std::endl;
                    std::cout << "Name (Student 2): " << student2.name << std::endl;
                    std::cout << "Year of Study (Student 2): " << student2.yearOfStudy << std::endl;

                    std::cout << "Room Preference: " << roomPreference << std::endl;
                    std::cout << "Fees Due: " << student.feesDue << std::endl;
                    std::cout << "Booking Date and Time: " << asctime(localTime);
                } else if (roomPreference == "Triple") {
    // For Triple sharing, ask for details of three students
    std::cout << "Enter details for Student 1:" << std::endl;
    std::cout << "Name: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character
    std::getline(std::cin, student.name);

    std::cout << "Enrollment number: ";
    std::cin >> student.enrollmentNumber;

    std::cout << "Year of study: ";
    std::cin >> student.yearOfStudy;

    // Create Student 2
    Student student2;
    std::cout << "Enter details for Student 2:" << std::endl;
    std::cout << "Name: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character
    std::getline(std::cin, student2.name);

    std::cout << "Enrollment number: ";
    std::cin >> student2.enrollmentNumber;

    std::cout << "Year of study: ";
    std::cin >> student2.yearOfStudy;

    // Create Student 3
    Student student3;
    std::cout << "Enter details for Student 3:" << std::endl;
    std::cout << "Name: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character
    std::getline(std::cin, student3.name);

    std::cout << "Enrollment number: ";
    std::cin >> student3.enrollmentNumber;

    std::cout << "Year of study: ";
    std::cin >> student3.yearOfStudy;

    // Continue with the common room booking logic
    std::cout << "Room booked successfully." << std::endl;

    // Generate receipt
    time_t now = time(0);
    tm* localTime = localtime(&now);

    std::cout << "\nReceipt:" << std::endl;
    std::cout << "Enrollment Number (Student 1): " << student.enrollmentNumber << std::endl;
    std::cout << "Name (Student 1): " << student.name << std::endl;
    std::cout << "Year of Study (Student 1): " << student.yearOfStudy << std::endl;

    std::cout << "Enrollment Number (Student 2): " << student2.enrollmentNumber << std::endl;
    std::cout << "Name (Student 2): " << student2.name << std::endl;
    std::cout << "Year of Study (Student 2): " << student2.yearOfStudy << std::endl;

    std::cout << "Enrollment Number (Student 3): " << student3.enrollmentNumber << std::endl;
    std::cout << "Name (Student 3): " << student3.name << std::endl;
    std::cout << "Year of Study (Student 3): " << student3.yearOfStudy << std::endl;

    std::cout << "Room Preference: " << roomPreference << std::endl;
    std::cout << "Fees Due: " << student.feesDue << std::endl;
    std::cout << "Booking Date and Time: " << asctime(localTime);
} else {
                    // Continue with the common room booking logic for other room types
                    std::cout << "Room booked successfully." << std::endl;

                    // Generate receipt
                    time_t now = time(0);
                    tm* localTime = localtime(&now);

                    std::cout << "\nReceipt:" << std::endl;
                    std::cout << "Enrollment Number: " << student.enrollmentNumber << std::endl;
                    std::cout << "Name: " << student.name << std::endl;
                    std::cout << "Room Preference: " << roomPreference << std::endl;
                    std::cout << "Fees Due: " << student.feesDue << std::endl;
                    std::cout << "Booking Date and Time: " << asctime(localTime);
                }
            } else {
                std::cout << "You already have a room booked." << std::endl;
            }

            return;
        }
    }

    std::cout << "Student not found with enrollment number " << enrollmentNumber << std::endl;
}

void registerStudent() {
    Student student;

    std::cout << "\nStudent Registration" << std::endl;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter your name: ";
    std::getline(std::cin, student.name);

    std::cout << "Enter your enrollment number: ";
    std::cin >> student.enrollmentNumber;

    std::cout << "Enter your year of study: ";
    std::cin >> student.yearOfStudy;

    std::cout << "Enter your mobile number: ";
    while (true) {
        std::cin >> student.mobileNumber;

        // Check if the entered mobile number has at least 10 digits
        if (student.mobileNumber.length() >= 10) {
            break; // Exit the loop if the condition is met
        } else {
            std::cout << "Invalid mobile number. Please enter a valid 10-digit mobile number: ";
        }
    }

    std::cout << "Enter your email address: ";
    std::cin >> student.email;

    std::cout << "Enter your guardian's name: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, student.guardianName);

    std::cout << "Enter your guardian's mobile number: ";
    while (true) {
        std::cin >> student.guardianMobileNumber;

        // Check if the entered guardian's mobile number has at least 10 digits
        if (student.guardianMobileNumber.length() >= 10) {
            break; // Exit the loop if the condition is met
        } else {
            std::cout << "Invalid guardian's mobile number. Please enter a valid 10-digit mobile number: ";
        }
    }

    std::cout << "Enter your home address: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, student.homeAddress);

    student.feesDue = 1000.0; // Initial fees due (you can set it as per your requirement)

    students.push_back(student);
    std::cout << "Registration successful." << std::endl;
}

int loginStudent() {
    int enrollmentNumber;
    std::cout << "Enter your enrollment number: ";
    std::cin >> enrollmentNumber;

    for (const Student& student : students) {
        if (student.enrollmentNumber == enrollmentNumber) {
            return enrollmentNumber;
        }
    }

    return -1; // Enrollment number not found
}

void viewStudentInfo() {
    std::cout << "\nStudent Information:" << std::endl;

    for (const Student& student : students) {
        std::cout << "Enrollment Number: " << student.enrollmentNumber << std::endl;
        std::cout << "Name: " << student.name << std::endl;
        std::cout << "Room Preference: " << student.roomPreference << std::endl;
        std::cout << "Fees Due: " << student.feesDue << std::endl << std::endl;
    }
}

void updateStudentInfo() {
    int enrollmentNumber;
    std::cout << "Enter your enrollment number: ";
    std::cin >> enrollmentNumber;

    for (Student& student : students) {
        if (student.enrollmentNumber == enrollmentNumber) {
            std::cout << "Enter updated name: ";
            std::cin.ignore();
            std::getline(std::cin, student.name);
            std::cout << "Information updated successfully." << std::endl;
            return;
        }
    }

    std::cout << "Student not found with enrollment number " << enrollmentNumber << std::endl;
}

void deleteStudentInfo() {
    int enrollmentNumber;
    std::cout << "Enter your enrollment number: ";
    std::cin >> enrollmentNumber;

    auto it = std::remove_if(students.begin(), students.end(),
                             [enrollmentNumber](const Student& student) {
                                 return student.enrollmentNumber == enrollmentNumber;
                             });

    if (it != students.end()) {
        students.erase(it, students.end());
        std::cout << "Information deleted successfully." << std::endl;
    } else {
        std::cout << "Student not found with enrollment number " << enrollmentNumber << std::endl;
    }
}

void addStudent() {
    Student student;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear newline character from previous input
    std::cout << "Enter student name: ";
    std::getline(std::cin, student.name);

    std::cout << "Enter enrollment number: ";
    std::cin >> student.enrollmentNumber;

    std::cout << "Enter room preference (Single/Double/Triple): ";
    std::cin >> student.roomPreference;

    std::cout << "Enter fees due: ";
    std::cin >> student.feesDue;

    students.push_back(student);
    std::cout << "Student added successfully." << std::endl;
}

void adminMenu() {
    int adminChoice;
    while (true) {
        std::cout << "\nAdmin Menu:" << std::endl;
        std::cout << "1. View Student Information" << std::endl;
        std::cout << "2. Update Student Information" << std::endl;
        std::cout << "3. Delete Student Information" << std::endl;
        std::cout << "4. Search Student by Enrollment Number" << std::endl;
        std::cout << "5. Add Student" << std::endl;
        std::cout << "6. View Room Booking Status" << std::endl;
        std::cout << "7. Return to Main Menu" << std::endl;
        std::cin >> adminChoice;

        switch (adminChoice) {
            case 1:
                viewStudentInfo();
                break;
            case 2:
                updateStudentInfo();
                break;
            case 3:
                deleteStudentInfo();
                break;
            case 4:
                searchStudentByEnrollment();
                break;
            case 5:
                addStudent();
                break;
            case 6:
                viewRoomStatus();
                break;
            case 7:
                return; // Return to main menu
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}
void viewRoomStatus() {
    std::cout << "\nRoom Booking Status:" << std::endl;

    // Create a vector to represent the room status (occupied/vacant)
    std::vector<std::string> roomStatus(60, "Vacant");

    // Check the status for each student
    for (const Student& student : students) {
        // Check if the roomPreference is not empty
        if (!student.roomPreference.empty()) {
            // Extract room type and number
            char roomType = student.roomPreference[0];
            int roomNumber = std::stoi(student.roomPreference.substr(2));

            // Update room status
            if (roomNumber >= 1 && roomNumber <= 60) {
                if (roomStatus[roomNumber - 1] == "Vacant") {
                    roomStatus[roomNumber - 1] = "Occupied";
                }
            }
        }
    }

    // Display room status
    for (int i = 0; i < 60; ++i) {
        std::cout << "Room ";
        if (i < 20) {
            std::cout << "S-" << i + 1;
        } else if (i < 40) {
            std::cout << "D-" << i - 19;
        } else {
            std::cout << "T-" << i - 39;
        }
        std::cout << ": " << roomStatus[i] << std::endl;
    }
}


void studentMenu(int enrollmentNumber) {
    int studentChoice;
    while (true) {
        std::cout << "\nStudent Menu:" << std::endl;
        std::cout << "1. Pay Fees" << std::endl;
        std::cout << "2. Book Room" << std::endl;
        std::cout << "3. Room Booking Status" << std::endl;  // Added option for room booking status
        std::cout << "4. Help Desk" << std::endl;
        std::cout << "5. Return to Main Menu" << std::endl;
        std::cin >> studentChoice;

        switch (studentChoice) {
            case 1:
                payFees(enrollmentNumber);
                break;
            case 2:
                bookRoom(enrollmentNumber);
                break;
            case 3:
                viewRoomStatus();  // Added function to view room booking status
                break;
            case 4:
                helpDesk();
                break;
            case 5:
                return; // Return to main menu
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}


void helpDesk() {
    std::string complaint;
    int complaintType;

    std::cout << "\nHelp Desk - Register Complaint" << std::endl;

    // Display complaint types
    std::cout << "Complaint Types:" << std::endl;
    std::cout << "1. Maintenance" << std::endl;
    std::cout << "2. Electrical" << std::endl;
    std::cout << "3. Housekeeping" << std::endl;
    std::cout << "4. Laundry" << std::endl;

    std::cout << "Choose the type of complaint (1-4): ";
    std::cin >> complaintType;

    // Validate complaint type
    if (complaintType < 1 || complaintType > 4) {
        std::cout << "Invalid complaint type. Please choose a valid type." << std::endl;
        return;
    }

    // Clear the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter your complaint or grievance: ";
    std::getline(std::cin, complaint);

    std::cout << "Complaint of type " << complaintType << " lodged successfully. Our team will get in touch with you." << std::endl;
}

void searchStudentByEnrollment() {
    int enrollmentNumber;
    std::cout << "Enter the enrollment number to search: ";
    std::cin >> enrollmentNumber;

    // Sort the students vector based on enrollmentNumber for binary search
    std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
        return a.enrollmentNumber < b.enrollmentNumber;
    });

    // Binary search
    auto it = std::lower_bound(students.begin(), students.end(), enrollmentNumber, [](const Student& a, int value) {
        return a.enrollmentNumber < value;
    });

    if (it != students.end() && it->enrollmentNumber == enrollmentNumber) {
        std::cout << "Student found:" << std::endl;
        std::cout << "Enrollment Number: " << it->enrollmentNumber << std::endl;
        std::cout << "Name: " << it->name << std::endl;
        std::cout << "Room Preference: " << it->roomPreference << std::endl;
        std::cout << "Fees Due: " << it->feesDue << std::endl;
    } else {
        std::cout << "Student not found with enrollment number " << enrollmentNumber << std::endl;
    }
}
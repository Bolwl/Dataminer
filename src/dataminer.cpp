#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>  // for std::find

// Callback function to write data received from CURL to a file
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* out = static_cast<std::ofstream*>(userp);
    size_t total_size = size * nmemb;
    out->write(static_cast<char*>(contents), total_size);
    return total_size;
}

// Progress callback function to display download progress
int ProgressCallback(void* clientp, curl_off_t total, curl_off_t downloaded, curl_off_t total_upload, curl_off_t uploaded) {
    float progress = (total > 0) ? (static_cast<float>(downloaded) / total) * 100.0f : 0;
    std::cout << "\rProgress: " << progress << "%" << std::flush;
    return 0; // Return 0 to continue the download
}

// Function to download a file from a given URL
void downloadFile(const std::string& url, const std::string& outputFile) {
    CURL* curl;
    CURLcode res;
    std::ofstream file(outputFile, std::ios::binary);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set URL and options for downloading the file
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Skip SSL peer verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Skip SSL host verification
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress meter
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback); // Set progress callback
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, nullptr); // Progress callback data (optional)

        // Perform the download
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "\ncurl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleanup CURL resources
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    file.close();
}

// Function to format item strings by removing specific substrings and formatting the text
std::string formatItem(const std::string& item) {
    std::string formatted = item;

    // Remove the "ITEM_ID_" prefix
    std::string toReplace = "ITEM_ID_";
    std::string::size_type pos = 0;
    while ((pos = formatted.find(toReplace, pos)) != std::string::npos) {
        formatted.erase(pos, toReplace.length());
    }

    // Replace underscores with spaces
    std::replace(formatted.begin(), formatted.end(), '_', ' ');

    // Capitalize the first letter of each word
    bool capitalize = true;
    for (char& c : formatted) {
        if (std::isspace(c)) {
            capitalize = true;
        } else if (capitalize) {
            c = std::toupper(c);
            capitalize = false;
        } else {
            c = std::tolower(c);
        }
    }

    return formatted;
}

// Function to truncate the last item in the list if it contains a specific delimiter (e.g., "ALIGNMENT")
void truncateLastItem(std::vector<std::string>& items, const std::string& delimiter) {
    if (!items.empty()) {
        std::string& lastItem = items.back();
        std::size_t pos = lastItem.find(delimiter);
        if (pos != std::string::npos) {
            lastItem = lastItem.substr(0, pos); // Truncate the string at the delimiter
        }
    }
}

// Function to remove non-ASCII characters from a string
std::string removeNonAscii(const std::string& text) {
    std::string result;
    for (char c : text) {
        // Only include characters in the ASCII range (printable)
        if (c >= 32 && c < 127) {
            result += c;
        }
    }
    return result;
}

// Function to extract item IDs from the file content
std::vector<std::string> extractItems(const std::string& content) {
    std::vector<std::string> items;
    std::istringstream stream(content);
    std::string line;

    // Parse each line of the content for item IDs
    while (std::getline(stream, line)) {
        size_t pos = 0;
        while ((pos = line.find("ITEM_ID_", pos)) != std::string::npos) {
            size_t endPos = line.find("ITEM_ID", pos + 1); // Find the next occurrence of "ITEM_ID"
            if (endPos == std::string::npos) endPos = line.length(); // If not found, use the end of the line
            std::string item = line.substr(pos, endPos - pos);
            items.push_back(removeNonAscii(item)); // Add the cleaned item to the list
            pos = endPos;
        }
    }

    truncateLastItem(items, "ALIGNMENT"); // Optionally truncate the last item

    return items;
}

// Function to display and save new items that are not present in the old item list
void displayNewItems(const std::vector<std::string>& items, const std::vector<std::string>& oldItems) {
    std::cout << "\n=========================================" << std::endl;
    std::cout << "Upcoming Item Names (Note: Some items may not be named properly)" << std::endl;
    std::cout << "=========================================" << std::endl;

    std::ofstream newItems("new_items.txt"); // File to store new items
    for (const std::string& item : items) {
        // If the item is not in the old item list, display and save it
        if (std::find(oldItems.begin(), oldItems.end(), item) == oldItems.end()) {
            std::string readableItem = formatItem(removeNonAscii(item));
            std::cout << readableItem << std::endl;
            newItems << readableItem << std::endl;
        }
    }

    newItems.close();
    std::cout << "Saved new items to 'new_items.txt'" << std::endl;
}

int main() {
    std::string version;
    std::cout << "Previous Version (Example: 4.64): ";
    std::cin >> version;

    // Read old items from a file based on the provided version
    std::ifstream oldFile("bol_V" + version + ".txt");
    if (!oldFile.is_open()) {
        std::cerr << "Previous version not found!" << std::endl;
        return 1;
    }

    std::vector<std::string> oldItems;
    std::string line;
    while (std::getline(oldFile, line)) {
        oldItems.push_back(line); // Add each line to the old items list
    }
    oldFile.close();

    // Download the latest Growtopia binary from the specified URL
    std::string url = "https://growtopiagame.com/Growtopia-mac.dmg";
    downloadFile(url, "Growtopia.dmg");

    // Extract the downloaded .dmg file using a system command (requires 7z or similar tool)
    system("\"7z.exe\" e Growtopia.dmg Growtopia.app/Contents/MacOS/Growtopia -aoa");

    // Read the extracted binary file
    std::ifstream extractedFile("Growtopia", std::ios::binary);
    if (!extractedFile.is_open()) {
        std::cerr << "Failed to open the extracted file!" << std::endl;
        return 1;
    }
    std::ostringstream contentStream;
    contentStream << extractedFile.rdbuf(); // Read the entire file content into a string
    std::string content = contentStream.str();
    extractedFile.close();

    // Extract items from the file content
    std::vector<std::string> items = extractItems(content);

    // Extract the new version string from the file content
    std::string versionStr = content.substr(content.find("www.growtopia1.com") + 18);
    versionStr = removeNonAscii(versionStr.substr(0, versionStr.find("Growtopia")));

    // Save the new items to a file named based on the new version
    std::ofstream newFile("bol_" + versionStr + ".txt");
    for (const std::string& item : items) {
        newFile << item << std::endl;
    }
    newFile.close();

    // Display and save the new items that were not in the previous version
    displayNewItems(items, oldItems);
    
    std::cout << "Press enter to exit...";
    std::cin.ignore(); // Ignore any previous input
    std::cin.get(); // Wait for the user to press enter

    return 0;
}

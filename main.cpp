#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <thread>
#include <future>
#include <numeric>
#include <windows.h>

uint16_t calculateModbusCRC(const std::vector<uint8_t>& data)
{
    static const uint8_t aCRCHi[] =
    {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
    };

    static const uint8_t aCRCLo[] =
    {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
        0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
        0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
        0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
        0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
        0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
        0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
        0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
        0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
        0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
        0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
        0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
        0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
        0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
        0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
    };

    uint8_t hi = 0xFF;
    uint8_t lo = 0xFF;
    size_t i = 0;

    while (i < data.size())
    {
        uint8_t index = hi ^ data[i++];
        hi = lo ^ aCRCHi[index];
        lo = aCRCLo[index];
    }

    return (hi << 8) | lo;
}

void calculateCRCBatch(const std::vector<uint8_t>& data, unsigned long start, unsigned long end, uint16_t& result)
{
    result = 0xFFFF;
    for (unsigned long i = start; i < end; ++i)
    {
        result = calculateModbusCRC(data);
    }
}

uint16_t calculateCRCMultithreaded(const std::vector<uint8_t>& data, unsigned long iterations)
{
    if (iterations <= 1)
    {
        return calculateModbusCRC(data);
    }

    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4; 

    numThreads = std::min(numThreads, 32u);

    numThreads = static_cast<unsigned int>(std::min(static_cast<unsigned long>(numThreads), iterations));

    std::vector<unsigned long> iterationsPerThread(numThreads, iterations / numThreads);

    for (unsigned int i = 0; i < iterations % numThreads; ++i)
    {
        iterationsPerThread[i]++;
    }

    std::vector<uint16_t> results(numThreads);
    std::vector<std::future<void>> futures;

    unsigned long startIter = 0;
    for (unsigned int i = 0; i < numThreads; ++i)
    {
        unsigned long endIter = startIter + iterationsPerThread[i];
        futures.push_back(std::async(std::launch::async, calculateCRCBatch, std::ref(data), startIter, endIter, std::ref(results[i])));
        startIter = endIter;
    }

    for (auto& future : futures)
    {
        future.wait();
    }

    return results.back();
}

std::vector<uint8_t> parseHexString(const std::string& input)
{
    std::vector<uint8_t> bytes;
    std::istringstream iss(input);
    std::string byteStr;

    while (iss >> byteStr)
    {
        byteStr.erase(std::remove_if(byteStr.begin(), byteStr.end(), [](char c) { return !isxdigit(c); }), byteStr.end());

        if (byteStr.empty()) continue;

        if (byteStr.length() % 2 != 0)
        {
            byteStr = "0" + byteStr;
        }

        for (size_t i = 0; i < byteStr.length(); i += 2)
        {
            std::string bytePart = byteStr.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoul(bytePart, nullptr, 16));
            bytes.push_back(byte);

            if (bytes.size() >= 256)
            {
                return bytes;
            }
        }
    }

    return bytes;
}

std::string filterHexInput(const std::string& input)
{
    std::string result;
    for (char c : input)
    {
        if (isxdigit(c) || c == ' ')
        {
            result += c;
        }
    }
    return result;
}

std::string filterNumericInput(const std::string& input)
{
    std::string result;
    for (char c : input)
    {
        if (isdigit(c))
        {
            result += c;
        }
    }
    return result;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 400 }), "Suma kontrolna CRC16");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("Arial.ttf"))
    {
        return EXIT_FAILURE;
    }

    sf::Text bytesLabel;
    bytesLabel.setFont(font);
    bytesLabel.setString("Bajty ramki:");
    bytesLabel.setCharacterSize(18);
    bytesLabel.setPosition({ 20, 20 });
    bytesLabel.setFillColor(sf::Color::White);

    sf::RectangleShape inputBytesBox(sf::Vector2f(560, 30));
    inputBytesBox.setPosition({ 20, 50 });
    inputBytesBox.setFillColor(sf::Color(50, 50, 50));
    inputBytesBox.setOutlineThickness(1);
    inputBytesBox.setOutlineColor(sf::Color::White);

    sf::Text inputBytesText;
    inputBytesText.setFont(font);
    inputBytesText.setString("");
    inputBytesText.setCharacterSize(18);
    inputBytesText.setPosition({ 25, 55 });
    inputBytesText.setFillColor(sf::Color::White);

    sf::Text repeatLabel;
    repeatLabel.setFont(font);
    repeatLabel.setString(L"Liczba powtórzeń:");
    repeatLabel.setCharacterSize(18);
    repeatLabel.setPosition({ 20, 100 });
    repeatLabel.setFillColor(sf::Color::White);

    sf::RectangleShape repeatCountBox(sf::Vector2f(560, 30));
    repeatCountBox.setPosition({ 20, 130 });
    repeatCountBox.setFillColor(sf::Color(50, 50, 50));
    repeatCountBox.setOutlineThickness(1);
    repeatCountBox.setOutlineColor(sf::Color::White);

    sf::Text repeatCountText;
    repeatCountText.setFont(font);
    repeatCountText.setString("1000000");
    repeatCountText.setCharacterSize(18);
    repeatCountText.setPosition({ 25, 135 });
    repeatCountText.setFillColor(sf::Color::White);

    sf::RectangleShape calculateButton(sf::Vector2f(560, 40));
    calculateButton.setPosition({ 20, 180 });
    calculateButton.setFillColor(sf::Color(0, 100, 200));
    calculateButton.setOutlineThickness(1);
    calculateButton.setOutlineColor(sf::Color::White);

    sf::Text calculateButtonText;
    calculateButtonText.setFont(font);
    calculateButtonText.setString("Oblicz CRC");
    calculateButtonText.setCharacterSize(20);
    calculateButtonText.setPosition({ 250, 190 });
    calculateButtonText.setFillColor(sf::Color::White);

    sf::Text resultsTitle;
    resultsTitle.setFont(font);
    resultsTitle.setString("Wyniki:");
    resultsTitle.setCharacterSize(20);
    resultsTitle.setPosition({ 20, 240 });
    resultsTitle.setFillColor(sf::Color::White);
    resultsTitle.setStyle(sf::Text::Bold);

    sf::Text crcLabel;
    crcLabel.setFont(font);
    crcLabel.setString("Suma kontrolna:");
    crcLabel.setCharacterSize(18);
    crcLabel.setPosition({ 20, 280 });
    crcLabel.setFillColor(sf::Color::White);

    sf::Text crcResult;
    crcResult.setFont(font);
    crcResult.setString("----");
    crcResult.setCharacterSize(18);
    crcResult.setPosition({ 200, 280 });
    crcResult.setFillColor(sf::Color::White);

    sf::Text totalTimeLabel;
    totalTimeLabel.setFont(font);
    totalTimeLabel.setString(L"Czas całkowity:");
    totalTimeLabel.setCharacterSize(18);
    totalTimeLabel.setPosition({ 20, 310 });
    totalTimeLabel.setFillColor(sf::Color::White);

    sf::Text totalTime;
    totalTime.setFont(font);
    totalTime.setString("--- ms");
    totalTime.setCharacterSize(18);
    totalTime.setPosition({ 200, 310 });
    totalTime.setFillColor(sf::Color::White);

    sf::Text iterationTimeLabel;
    iterationTimeLabel.setFont(font);
    iterationTimeLabel.setString("Czas iteracji:");
    iterationTimeLabel.setCharacterSize(18);
    iterationTimeLabel.setPosition({ 20, 340 });
    iterationTimeLabel.setFillColor(sf::Color::White);

    sf::Text iterationTime;
    iterationTime.setFont(font);
    iterationTime.setString("--- ms");
    iterationTime.setCharacterSize(18);
    iterationTime.setPosition({ 200, 340 });
    iterationTime.setFillColor(sf::Color::White);

    bool inputBytesActive = false;
    bool repeatCountActive = false;
    std::string inputBytesStr;
    std::string repeatCountStr = "1000000";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    inputBytesActive = inputBytesBox.getGlobalBounds().contains(mousePos);
                    repeatCountActive = repeatCountBox.getGlobalBounds().contains(mousePos);

                    if (calculateButton.getGlobalBounds().contains(mousePos))
                    {
                        try {
                            unsigned long n = std::stoul(repeatCountStr);
                            if (n < 1 || n > 1000000000) {
                                crcResult.setString(L"Błąd: zla liczba powtórzeń");
                                continue;
                            }

                            std::vector<uint8_t> data = parseHexString(inputBytesStr);
                            if (data.empty()) {
                                crcResult.setString(L"Błąd: brak danych");
                                continue;
                            }

                            auto start = std::chrono::high_resolution_clock::now();

                            uint16_t crc = calculateCRCMultithreaded(data, n);

                            auto end = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                            uint8_t lowByte = crc & 0xFF;
                            uint8_t highByte = (crc >> 8) & 0xFF;

                            std::stringstream crcSS;
                            crcSS << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(highByte) << std::setw(2) << std::setfill('0') << static_cast<int>(lowByte);
                            crcResult.setString(crcSS.str());

                            totalTime.setString(std::to_string(duration) + " ms");

                            std::stringstream iterSS;
                            iterSS << std::fixed << std::setprecision(9) << (duration / static_cast<double>(n)) << " ms";
                            iterationTime.setString(iterSS.str());
                        }
                        catch (const std::exception& e)
                        {
                            crcResult.setString("Blad obliczen: " + std::string(e.what()));
                        }
                        catch (...)
                        {
                            crcResult.setString(L"Nieznany błąd obliczeń");
                        }
                    }
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (inputBytesActive) {
                    if (event.text.unicode == '\b')
                    {
                        if (!inputBytesStr.empty())
                        {
                            inputBytesStr.pop_back();
                        }
                    }
                    else if (event.text.unicode < 128)
                    {
                        char c = static_cast<char>(event.text.unicode);
                        if (isxdigit(c) || c == ' ')
                        {
                            inputBytesStr += c;
                        }
                    }
                    inputBytesText.setString(inputBytesStr);
                }
                else if (repeatCountActive)
                {
                    if (event.text.unicode == '\b')
                    {
                        if (!repeatCountStr.empty())
                        {
                            repeatCountStr.pop_back();
                        }
                    }
                    else if (event.text.unicode >= '0' && event.text.unicode <= '9')
                    {
                        repeatCountStr += static_cast<char>(event.text.unicode);
                    }
                    repeatCountText.setString(repeatCountStr);
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::V)
            {
                sf::String clipboard = sf::Clipboard::getString();
                std::string clipboardText = clipboard.toAnsiString();

                if (inputBytesActive)
                {
                    inputBytesStr += filterHexInput(clipboardText);
                    inputBytesText.setString(inputBytesStr);
                }
                else if (repeatCountActive)
                {
                    repeatCountStr += filterNumericInput(clipboardText);
                    repeatCountText.setString(repeatCountStr);
                }
            }
        }

        inputBytesBox.setOutlineColor(inputBytesActive ? sf::Color::Yellow : sf::Color::White);
        repeatCountBox.setOutlineColor(repeatCountActive ? sf::Color::Yellow : sf::Color::White);

        window.clear(sf::Color(30, 30, 30));

        window.draw(bytesLabel);
        window.draw(inputBytesBox);
        window.draw(inputBytesText);
        window.draw(repeatLabel);
        window.draw(repeatCountBox);
        window.draw(repeatCountText);
        window.draw(calculateButton);
        window.draw(calculateButtonText);
        window.draw(resultsTitle);
        window.draw(crcLabel);
        window.draw(crcResult);
        window.draw(totalTimeLabel);
        window.draw(totalTime);
        window.draw(iterationTimeLabel);
        window.draw(iterationTime);

        window.display();
    }

    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main();
}
#endif
// Nermien Elassy
// 900196006

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <float.h>
#include <unordered_map>
#include <sstream>
#include <map>
#include <queue>
#include <list>
#include <set>
#include <limits>
#include <cmath>

using namespace std;
struct web_data
{
    int impressions;
    int clicks;
    double rank;
    double score;
    int CTR;
};
// Global Variables
unordered_map<string, vector<string>> webGraph;
unordered_map<string, vector<string>> keyWord;
unordered_map<string, web_data> webInfo;
map<string, int> visited;
vector<string> finalScore;
vector<string> words;
vector<string> websites;

// Seach Function Declaration
void SearchQuery();

// Function to read first file and create the webgraph
void read_webgraph()
{
    string line, web_name, word;
    fstream webFile;
    webFile.open("web_graph.csv", ios::in);
    if (webFile.fail())
    {
        cout << "Error: Cannot open web graph file"
             << "\n";
        exit(0);
    }
    while (!webFile.eof())
    {
        getline(webFile, line);
        stringstream s(line);
        getline(s, web_name, ',');
        while (getline(s, word, ','))
        {
            if (word.size() < 2)
                break;
            webGraph[web_name].push_back(word);
        }
    }
}

// Function to read second file and create list of keyword
void read_keyWord()
{
    string line, web_name, word;
    fstream KeyFile;
    KeyFile.open("keyWord.csv", ios::in);
    if (KeyFile.fail())
    {
        cout << "Error: Cannot open KeyWord file"
             << "\n";
        exit(0);
    }

    while (!KeyFile.eof())
    {
        getline(KeyFile, line, '\n');
        stringstream s(line);
        getline(s, web_name, ',');
        // websites.push_back(fword);
        while (getline(s, word, ','))
        {
            // for (int i = 0; i < word.size(); i++)
            // {
            //     word[i] = tolower(word[i]);
            // }
            if (word.size() < 2)
                break;
            keyWord[web_name].push_back(word);
        }
    }
}
// Function to read third file and initialize number of impressions
void read_impressions()
{
    string line, web_name, word;
    fstream ImpFile;
    ImpFile.open("Impressions.csv", ios::in);
    if (ImpFile.fail())
    {
        cout << "Error: Cannot open Impressions file"
             << "\n";
        exit(0);
    }
    while (!ImpFile.eof())
    {
        getline(ImpFile, line);
        stringstream s(line);
        getline(s, web_name, ',');

        while (getline(s, word, ','))
        {
            if (word.size() < 2 && word == " ")
                break;
            int temp;
            temp = stoi(word);

            webInfo[web_name].impressions = temp;
        }
    }
}
void update_impressions()
{
    ofstream file("Impressions.csv");
    for (auto &it : webInfo)
    {
        file << it.first << "," << it.second.impressions << ",\n";
    }
    file.close();
}
void Initial_PageRank()
{
    for (auto &it : webInfo)
    {
        it.second.rank = 1.0 / webInfo.size();
    }
}

void Normalize()
{
    double temp_rank;
    string temp_web;
    double max_prev = DBL_MAX;
    for (auto &it : webInfo)
        visited[it.first] = 0;
    for (int i = (int)webInfo.size() - 1; i >= 1; i--)
    {
        temp_rank = 0.0;
        for (auto &it : webInfo)
        {
            if (it.second.rank >= temp_rank && it.second.rank < max_prev && !visited[it.first])
            {
                temp_rank = it.second.rank;
                temp_web = it.first;
            }
        }
        visited[temp_web] = 1;
        webInfo[temp_web].rank = i;
        max_prev = temp_rank;
    }
    // Normalizing page rank
    for (auto &it : webInfo)
        it.second.rank = (double)(it.second.rank - 1) / ((int)webInfo.size() - 1); // new value = (value - min)/(max - min)
}

void ClaculateLoop()
{
    const double error = 0.00001;
    double page_rank;
    unordered_map<string, double> prev_rank;
    for (auto &it : webInfo)
    {
        prev_rank[it.first] = it.second.rank;
    }

    const int MaxIterations = 1000;
    int iterations = 0;
    while (iterations++ < MaxIterations)
    {
        int crr = 0;
        for (auto &it : webInfo)
        {
            visited[it.first] = 0;
        }

        for (auto &parent : webInfo)
        {
            for (auto &children : webGraph[parent.first])
            {
                page_rank = parent.second.rank / webGraph[parent.first].size();
                if (!visited[children])
                {
                    prev_rank[children] = page_rank;
                    visited[children] = 1;
                }
                else
                    prev_rank[children] += page_rank;
            }
        }
        for (auto &it : webInfo)
        {
            if (abs(it.second.rank - prev_rank[it.first]) < error)
                crr++;
        }
        for (auto &it : webInfo)
            it.second.rank = prev_rank[it.first];
        if (crr == webInfo.size() - 1)
            break;
    }
}

void Update_PageRank()
{
    ClaculateLoop();
    Normalize();
}
void Initial_CTR()
{
    for (auto &it : webInfo)
        it.second.CTR = 1.0 / it.second.impressions;
}
void Update_CTR()
{
    for (auto &it : webInfo)
        it.second.CTR = ((double)it.second.clicks / it.second.impressions);
}

void Initial_score()
{
    for (auto &it : webInfo)
    {
        it.second.score = (0.4 * it.second.rank) + ((1 - (0.1 * it.second.impressions / 1 + 0.1 * it.second.impressions)) * it.second.rank + (0.1 * it.second.impressions / 1 + 0.1 * it.second.impressions) * it.second.CTR) * 0.6;
    }
}
void Update_Score()
{
    for (auto &it : webInfo)
    {
        it.second.score = (0.4 * it.second.rank) + ((1 - (0.1 * it.second.impressions / 1 + 0.1 * it.second.impressions)) * it.second.rank + (0.1 * it.second.impressions / 1 + 0.1 * it.second.impressions) * it.second.CTR) * 0.6;
    }
}
void Clicks(string clicked)
{
    for (auto &it : webInfo)
    {
        if (it.first == clicked)
            it.second.clicks++;
    }
}
void screen1()
{
    if (websites.size() == 0)
    {
        int choice;
        cout << "No Webpages found \n";
        cout << "1.New Search\n";
        cout << "2.Exit\n";
        cin >> choice;
        // Validation
        if (choice != 1 && choice != 2)
        {
            cout << "Enter a valid choice\n";
            cin >> choice;
        }
        while (choice != 1 && choice != 2)
        {
            cout << "Enter a valid choice\n";
            cin >> choice;
        }
        if (choice == 1)
            SearchQuery();
        else if (choice == 2)
        {
            update_impressions();
            exit(0);
        }
    }
    int counter = 1;
    int main_choice;
    int web_choice;
    vector<string> web_names;
    cout << "Search Results:"
         << "\n";
    for (auto &it : finalScore)
    {
        web_names.push_back(it);
        cout << counter << "." << it << "\n";
        counter++;
    }
    cout << "Would you like to:"
         << "\n";
    cout << "1.Choose a Webpage to open"
         << "\n";
    cout << "2.New Search"
         << "\n";
    cout << "3.Exit\n";
    cin >> main_choice;
    // Validation
    if (main_choice != 1 && main_choice != 2 && main_choice != 3)
    {
        cout << "Enter a valid choice\n";
        cin >> main_choice;
    }
    while (main_choice != 1 && main_choice != 2 && main_choice != 3)
    {
        cout << "Enter a valid choice\n";
        cin >> main_choice;
    }
    string temp;
    if (main_choice == 1)
    {
        cout << "Please enter the index of your chosen webpage"
             << "\n";
        cin >> web_choice;
        // Validation
        if (web_choice <= 0 || web_choice > web_names.size())
        {
            cout << "Enter a valid choice\n";
            cin >> web_choice;
        }
        while (web_choice <= 0 || web_choice > web_names.size())
        {
            cout << "Enter a valid choice\n";
            cin >> web_choice;
        }
        temp = web_names[web_choice - 1];
        Clicks(temp);
        Update_CTR();
        Update_Score();

        int choice;
        cout << "You are now viewing " << temp << "\n";
        cout << "Would you like to "
             << "\n";
        cout << "1.Return to Search Results"
             << "\n";
        cout << "2.New Search"
             << "\n";
        cout << "3.Exit"
             << "\n";
        cin >> choice;
        // Validation
        if (choice != 1 && choice != 2 && choice != 3)
        {
            cout << "Enter a valid choice\n";
            cin >> choice;
        }
        while (choice != 1 && choice != 2 && choice != 3)
        {
            cout << "Enter a valid choice\n";
            cin >> choice;
        }
        if (choice == 1)
            screen1();
        if (choice == 2)
            SearchQuery();
        if (choice == 3)
        {
            update_impressions();
            exit(0);
        }
    }
    if (main_choice == 2)
        SearchQuery();
    else if (main_choice == 3)
    {
        update_impressions();
        exit(0);
    }
}
// Search Query
void SearchQuery()
{
    words.clear();
    websites.clear();

    string Query;
    cout << "Search: "
         << "\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(cin, Query);

    int type = 3;
    // Case 1
    if (Query[0] == '\"')
    {
        type = 1;
        words.push_back(Query.substr(1, Query.size() - 2));
        for (auto &it : keyWord)
        {
            auto result = find(it.second.begin(), it.second.end(), words.back());
            if (result != it.second.end())
            {
                websites.push_back(it.first);
            }
            if (websites.size() > 0)
            {
                webInfo[websites.back()].impressions++;
            }
        }
    }

    else
    {
        for (int i = 0; i < Query.size(); i++)
        {
            Query[i] = tolower(Query[i]);
        }
        stringstream line(Query);
        string temp;
        while (line >> temp)
        {
            if (temp == "and")
            {
                type = 2;
                continue;
            }
            else if (temp == "or")
            {
                type = 3;
                continue;
            }
            words.push_back(temp);
        }
    }

    // Type 2
    if (type == 2)
    {
        bool flag;
        // Check if the whole words vector present in one or more websites
        for (auto it : keyWord)
        {
            vector<string> keywords;
            for (int i = 0; i < it.second.size(); i++)
            {
                string word = it.second[i];
                for (int j = 0; j < word.size(); j++)
                    word[i] = tolower(word[i]);

                keywords.push_back(word);
            }
            flag = false;
            for (int i = 0; i < (int)words.size(); i++)
            {
                auto result = find(keywords.begin(), keywords.end(), words[i]);
                if (result == keywords.end())
                {
                    flag = true;
                    break;
                }
            }
            if (flag == false)
            {
                websites.push_back(it.first);
                webInfo[websites.back()].impressions++;
            }
        }
    }

    // Type 3
    // Check if any of the words vector present in one or more websites
    if (type == 3)
    {
        set<string> webs;
        // Check if the whole words vector present in one or more websites
        for (int i = 0; i < (int)words.size(); i++)
        {
            for (auto it : keyWord)
            {
                vector<string> keywords;
                for (int k = 0; k < it.second.size(); k++)
                {
                    string word = it.second[k];
                    for (int j = 0; j < word.size(); j++)
                        word[i] = tolower(word[i]);

                    keywords.push_back(word);
                }
                auto result = find(keywords.begin(), keywords.end(), words[i]);
                if (result != keywords.end())
                {
                    webs.insert(it.first);
                }
            }
        }
        for (auto &element : webs)
        {
            websites.push_back(element);
            webInfo[element].impressions++;
        }
    }

    finalScore.clear();
    Update_CTR();
    Update_Score();

    priority_queue<double, vector<double>, less<double>> queue;
    map<double, string> scoresMap;
    for (int i = 0; i < websites.size(); i++)
    {
        scoresMap[webInfo[websites[i]].score] = websites[i];
        queue.push(webInfo[websites[i]].score);
    }

    while (queue.empty() == false)
    {
        auto element = queue.top();
        string website = scoresMap[element];
        finalScore.push_back(website);
        queue.pop();
    }

    screen1();
}

// Menus Program
void Menu()
{
    int choice;
    Initial_PageRank();
    Update_PageRank();
    Initial_CTR();
    Initial_score();
    cout << "Welcome!"
         << "\n";
    cout << "What would you like to do?"
         << "\n";
    cout << "1.New Search"
         << "\n";
    cout << "2.Exit"
         << "\n";
    cout << "Type your choice: "
         << "\n";

    cin >> choice;
    // Validation
    if (choice != 1 && choice != 2)
    {
        cout << "Enter a valid choice\n";
        cin >> choice;
    }
    while (choice != 1 && choice != 2)
    {
        cout << "Enter a valid choice\n";
        cin >> choice;
    }

    if (choice == 1)
        SearchQuery();
    else if (choice == 2)
    {
        update_impressions();
        exit(0);
    }
}
int main()
{
    read_webgraph();
    read_keyWord();
    read_impressions();
    Menu();
    return 0;
}

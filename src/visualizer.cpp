#include <ncurses.h>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>



namespace sort {
    template<typename Container, typename Compare = std::less<>>
    void bubbleSort(Container&& container, Compare comp = Compare()) {
        for(size_t i = 0; i < container.size() - 1; ++i) {
            for(size_t j = 0; j < container.size() - 1; ++j) { 
                if(comp(container[j + 1], container[j])) {
                    std::swap(container[j], container[j + 1]);
                }
            }
        }
    }
    
    template<typename Container, typename Compare = std::less<>>
    void insertSort(Container&& container, Compare comp = Compare()) {
        for(size_t i = 1; i < container.size(); ++i) {
            for(size_t j = i; j != 0 && comp(container[j], container[j - 1]); --j) {
                std::swap(container[j], container[j - 1]);
            }
        }
    }
    
    template<typename Container, typename Compare = std::less<>>
    void selectionSort(Container&& container, Compare comp = Compare()) {
        for(size_t i = 0; i < container.size(); ++i) {
            auto min = container[i];
            size_t minPosition = i;
            for(size_t j = i + 1; j < container.size(); ++j) {
                if(container[j] < min) {
                    min = container[j];
                    minPosition = j;
                }
            }
            if(i != minPosition) {
                std::swap(container[i], container[minPosition]);
            }
        }
    }
}

class SortVisualizer {
private:
    std::vector<int> data;
    std::vector<int> original_data;
    int width, height;
    int array_size;
    int delay_ms;
    
    enum Colors {
        NORMAL_PAIR = 1,
        COMPARING_PAIR = 2,
        SWAPPING_PAIR = 3,
        SORTED_PAIR = 4,
        PIVOT_PAIR = 5
    };
    
public:
    SortVisualizer(int size = 50, int delay = 50) 
        : array_size(size), delay_ms(delay) {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);
        
        if (has_colors()) {
            start_color();
            init_pair(NORMAL_PAIR, COLOR_WHITE, COLOR_BLUE);
            init_pair(COMPARING_PAIR, COLOR_BLACK, COLOR_YELLOW);
            init_pair(SWAPPING_PAIR, COLOR_WHITE, COLOR_RED);
            init_pair(SORTED_PAIR, COLOR_BLACK, COLOR_GREEN);
            init_pair(PIVOT_PAIR, COLOR_WHITE, COLOR_MAGENTA);
        }
        
        getmaxyx(stdscr, height, width);
        generateRandomData();
    }
    
    ~SortVisualizer() {
        endwin();
    }
    
    void generateRandomData() {
        data.clear();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, height - 10);
        
        for (int i = 0; i < array_size; ++i) {
            data.push_back(dis(gen));
        }
        original_data = data;
    }
    
    void drawArray(const std::vector<int>& highlight_indices = {}, 
                   const std::vector<int>& colors = {}) {
        clear();
        
        mvprintw(0, (width - 30) / 2, "SORT ALGORITHM VISUALIZER");
        mvprintw(1, (width - 40) / 2, "Press 'q' to quit, 'r' to reset, 1-6 for algorithms");
        
        int bar_width = std::max(1, (width - 10) / array_size);
        int start_x = (width - (bar_width * array_size)) / 2;
        
        for (size_t i = 0; i < data.size(); ++i) {
            int bar_height = data[i];
            int x = start_x + i * bar_width;
            
            int color_pair = NORMAL_PAIR;
            for (size_t j = 0; j < highlight_indices.size(); ++j) {
                if (static_cast<int>(i) == highlight_indices[j] && j < colors.size()) {
                    color_pair = colors[j];
                    break;
                }
            }
            
            if (has_colors()) {
                attron(COLOR_PAIR(color_pair));
            }
            
            for (int y = 0; y < bar_height; ++y) {
                for (int bx = 0; bx < bar_width; ++bx) {
                    mvaddch(height - 3 - y, x + bx, ' ');
                }
            }
            
            if (has_colors()) {
                attroff(COLOR_PAIR(color_pair));
            }
        }
        
        mvprintw(height - 2, 2, "Array size: %d", array_size);
        mvprintw(height - 1, 2, "Delay: %dms", delay_ms);
        
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    
    void visualBubbleSort() {
        for (size_t i = 0; i < data.size() - 1; ++i) {
            for (size_t j = 0; j < data.size() - 1 - i; ++j) {
                drawArray({static_cast<int>(j), static_cast<int>(j + 1)}, 
                         {COMPARING_PAIR, COMPARING_PAIR});
                
                if (data[j] > data[j + 1]) {
                    drawArray({static_cast<int>(j), static_cast<int>(j + 1)}, 
                             {SWAPPING_PAIR, SWAPPING_PAIR});
                    std::swap(data[j], data[j + 1]);
                }
            }
            drawArray({static_cast<int>(data.size() - 1 - i)}, {SORTED_PAIR});
        }
        drawArray();
    }
    
    void visualInsertionSort() {
        for (size_t i = 1; i < data.size(); ++i) {
            int key = data[i];
            int j = i - 1;
            
            drawArray({static_cast<int>(i)}, {PIVOT_PAIR});
            
            while (j >= 0 && data[j] > key) {
                drawArray({j, j + 1}, {COMPARING_PAIR, SWAPPING_PAIR});
                data[j + 1] = data[j];
                j--;
            }
            data[j + 1] = key;
            
            drawArray({j + 1}, {SORTED_PAIR});
        }
        drawArray();
    }
    
    void visualSelectionSort() {
        for (size_t i = 0; i < data.size(); ++i) {
            size_t min_idx = i;
            
            for (size_t j = i + 1; j < data.size(); ++j) {
                drawArray({static_cast<int>(min_idx), static_cast<int>(j)}, 
                         {PIVOT_PAIR, COMPARING_PAIR});
                
                if (data[j] < data[min_idx]) {
                    min_idx = j;
                }
            }
            
            if (min_idx != i) {
                drawArray({static_cast<int>(i), static_cast<int>(min_idx)}, 
                         {SWAPPING_PAIR, SWAPPING_PAIR});
                std::swap(data[i], data[min_idx]);
            }
            
            drawArray({static_cast<int>(i)}, {SORTED_PAIR});
        }
        drawArray();
    }
    
    void visualQuickSort(int low = 0, int high = -1) {
        if (high == -1) high = data.size() - 1;
        
        if (low < high) {
            int pi = partition(low, high);
            visualQuickSort(low, pi - 1);
            visualQuickSort(pi + 1, high);
        }
    }
    
private:
    int partition(int low, int high) {
        int pivot = data[high];
        int i = low - 1;
        
        drawArray({high}, {PIVOT_PAIR});
        
        for (int j = low; j < high; ++j) {
            drawArray({j, high}, {COMPARING_PAIR, PIVOT_PAIR});
            
            if (data[j] < pivot) {
                i++;
                if (i != j) {
                    drawArray({i, j}, {SWAPPING_PAIR, SWAPPING_PAIR});
                    std::swap(data[i], data[j]);
                }
            }
        }
        
        drawArray({i + 1, high}, {SWAPPING_PAIR, SWAPPING_PAIR});
        std::swap(data[i + 1], data[high]);
        
        return i + 1;
    }

public:
    void showMenu() {
        clear();
        mvprintw(5, 10, "=== SORT ALGORITHM VISUALIZER ===");
        mvprintw(7, 10, "1. Bubble Sort");
        mvprintw(8, 10, "2. Insertion Sort");
        mvprintw(9, 10, "3. Selection Sort");
        mvprintw(10, 10, "4. Quick Sort");
        mvprintw(11, 10, "5. Generate New Data");
        mvprintw(12, 10, "6. Adjust Speed");
        mvprintw(14, 10, "Press 'q' to quit");
        mvprintw(16, 10, "Choose an option: ");
        refresh();
    }
    
    void adjustSpeed() {
        clear();
        mvprintw(5, 10, "Current delay: %dms", delay_ms);
        mvprintw(6, 10, "Enter new delay (10-1000ms): ");
        refresh();
        
        echo();
        char input[10];
        getstr(input);
        noecho();
        
        int new_delay = std::atoi(input);
        if (new_delay >= 10 && new_delay <= 1000) {
            delay_ms = new_delay;
        }
    }
    
    void run() {
        int choice;
        
        while (true) {
            showMenu();
            choice = getch();
            
            switch (choice) {
                case '1':
                    data = original_data;
                    mvprintw(18, 10, "Running Bubble Sort...");
                    refresh();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    visualBubbleSort();
                    break;
                    
                case '2':
                    data = original_data;
                    mvprintw(18, 10, "Running Insertion Sort...");
                    refresh();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    visualInsertionSort();
                    break;
                    
                case '3':
                    data = original_data;
                    mvprintw(18, 10, "Running Selection Sort...");
                    refresh();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    visualSelectionSort();
                    break;
                    
                case '4':
                    data = original_data;
                    mvprintw(18, 10, "Running Quick Sort...");
                    refresh();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    visualQuickSort();
                    break;
                    
                case '5':
                    generateRandomData();
                    mvprintw(18, 10, "Generated new random data!");
                    refresh();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    break;
                    
                case '6':
                    adjustSpeed();
                    break;
                    
                case 'q':
                case 'Q':
                    return;
                    
                default:
                    mvprintw(18, 10, "Invalid choice! Press any key to continue...");
                    refresh();
                    getch();
                    break;
            }
        }
    }
};

int main() {
    try {
        SortVisualizer visualizer(40, 100); 
        visualizer.run();
    } catch (const std::exception& e) {
        endwin();
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

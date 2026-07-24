# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -I./include -O2
LDFLAGS = -lncurses -lm

# Директории
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
TARGET = life

# Автоматически находим все .c файлы в src/
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Правило по умолчанию
all: $(TARGET)

# Линковка
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Компиляция .c -> .o (с автоматическим созданием obj/)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Создание папки obj/, если её нет
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Для отладки (добавляем -g и отключаем оптимизацию)
debug: CFLAGS += -g -DDEBUG -O0
debug: clean $(TARGET)

# Объявляем цели, которые не являются файлами
.PHONY: all clean run debug

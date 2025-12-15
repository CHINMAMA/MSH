# MSH
MSH - это эмулятор для языка оболочки ОС. Работа эмулятора приближена к работе в командной строке UNIX-подобной ОС.
<img width="1357" height="360" alt="image" src="https://github.com/user-attachments/assets/7b0e4e15-0cd1-498e-b1b1-c3cb3dfe0826" />

## Сборка и запуск
```
sudo apt update
sudo apt install libxml2
make
./build/bin/msh
```

Доступные цели make:
all       - Собрать проект (по умолчанию) \
test      - Запуск тестов \
clean     - Очистка build \
help      - Помощь

## API
### struct MSH_ShellConfig
const char *VFS_path - Путь к источнику виртуальной файловой системы. \
const char *startup_script - Путь к стартовому скрипту.

### void MSH_Loop(MSH_ShellConfig config) 
Инициализация оболочки и запуск цикла CLI.

### void MSH_CreateContext(void)
Инициализация структуры глобального контекста оболочки (struct MSH_ExecContext).

### struct MSH_ExecContext
VFS* vfs - Указатель на объект виртуальной файловой системы. \
MSH_History history - Объект истории ввода. \
MSH_Env* env - Указатель на объект окружения (хеш-таблица переменных окружения). \
char *cwd - Текучащая директория внутри VFS. \
unsigned char running - Bool. При значении False программа останавливается.

### void MSH_FreeContext(void)
Освобождение памяти выделенной для глобального контекста и его составляющих.

### void MSH_CreateHistory(size_t buf_size)
Инициализация объекта истории.

### void MSH_AddToHistory(const char *line)
Добавление строки в историю.

### void MSH_FlushHistory(void)
Очистка истории ввода.

### void MSH_ClearHistory(void)
Очистка истории ввода и освобождение памяти.

### void MSH_ExecuteCommand(const char **args)
Выполнение встроенной команды.

### struct MSH_Token
MSH_TokenType type - тип токена. \
char *str - содержимое токена. \
unsigned char owns - владеет ли токен памятью (если False, то за освобождение памяти MSH_Token не ответственен). \
MSH_ExpansionType exp_type - тип токена подстановки (expansion token).

### MSH_Token *MSH_SplitLine(char *line)
Разбиение ввода на токены.

### void MSH_ExpandVariables(char **line)
Подстановка значений переменных.

### char **MSH_ParseTokens(MSH_Token *tokens)
Преобразование токенов в массив строчных аргументов.

### void MSH_CreateEnv(size_t size)
Инициализация окружения.

### void MSH_FreeEnv()
Освобождение памяти окружения.

### void MSH_InsertVar(const char *name, const char *val)
Создание переменной окружения.

### unsigned char MSH_HasVar(const char *name)
Проверка на существование переменной окружения.

### const char *MSH_GetVar(const char *name)
Получение значения переменной окружения

### void VFS_Create()
Инициализация объекта виртуальной файловой системы.

### void VFS_LoadXML(const char *path)
Загрузка источника VFS.

### void VFS_Serialize(const char *path)
Сериализация объекта VFS.

### xmlNodePtr VFS_Locate(const char *path)
Поиск узла VFS (папки, файла) по абсолютному/относительному пути.

### char *VFS_GetPath(xmlNodePtr node)
Получение абсолютного пути к узлу.

### void VFS_Mkdir(const char *path)
Создание узла папки в VFS.

### void VFS_Rmdir(const char *path)
Удаление узла в VFS.

### void VFS_Touch(const char *path)
Создание узла файла в VFS.

### char *VFS_GetCWD(void)
Получение текущей директории.

### void VFS_Clear(void);

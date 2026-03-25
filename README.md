# Archero Clone

Top-down roguelike shooter, вдохновлённый игрой Archero. Написан на C++17 с использованием SFML 3.0.2.

## Требования (macOS)

- macOS 12+ (Apple Silicon или Intel)
- CMake 3.10+
- Компилятор C++17 (Clang из Xcode Command Line Tools)

## Установка и запуск

### 1. Установить инструменты сборки

```bash
xcode-select --install
brew install cmake
```

> Если Homebrew не установлен: https://brew.sh

### 2. Клонировать репозиторий

```bash
git clone <URL_РЕПОЗИТОРИЯ>
cd ArcheroClone
```

### 3. Собрать проект

```bash
mkdir -p build && cd build
cmake ..
make -j$(sysctl -n hw.logicalcpu)
```

### 4. Запустить игру

```bash
./ArcheroClone
```

> Игра запускается в полноэкранном режиме. Для выхода — ESC или кнопка EXIT в меню.

## Управление

| Клавиша | Действие |
|---------|----------|
| W / ↑ | Движение вверх |
| A / ← | Движение влево |
| S / ↓ | Движение вниз |
| D / → | Движение вправо |
| Enter / Space | Выбор пункта меню |
| ESC | Назад / выход в меню |

Персонаж **автоматически стреляет** по ближайшему врагу, когда стоит на месте.

## Структура проекта

```
ArcheroClone/
├── src/            # Исходный код (.cpp)
├── include/        # Заголовочные файлы (.h)
├── assets/         # Спрайты, звуки, шрифты, музыка
├── libs/SFML-3.0.2 # SFML (поставляется вместе с проектом)
├── CMakeLists.txt  # Конфигурация сборки
└── README.md
```

## Примечание

SFML 3.0.2 уже включён в репозиторий (`libs/`), отдельная установка не требуется.
// Update

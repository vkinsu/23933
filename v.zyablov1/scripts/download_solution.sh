#!/bin/bash

# Функция для вывода текста зелёным цветом
echo_green() {
    local text="$1"
    echo -e "\033[0;32m$text\033[0m"
}

# Проверка, что вызов идет из корня проекта
if [ ! -d "scripts" ] || [ ! -f "scripts/download_solution.sh" ]; then
    echo "Скрипт должен быть вызван из корня проекта, где находится папка 'scripts'."
    exit 1
fi

# Проверка количества аргументов
if [ "$#" -ne 3 ]; then
    echo_green "Использование: {последнюю цифру номера группы} {имя пользователя} \"{путь к решению}\""
    exit 1
fi

i_group=$1
username=$2
path_to_solution=$3

# Проверка соответствия пути шаблону "имя_папки/название_файла.c"
if [[ ! $path_to_solution =~ ^[a-zA-Z_]+[0-9]+/[a-zA-Z0-9_]+\.c$ ]]; then
    echo "Ошибка: путь к решению должен соответствовать шаблону \"имя_папки/название_файла.c\"."
    exit 1
fi

# Извлечение имени папки и имени файла
folder_name=$(dirname "$path_to_solution")
file_name=$(basename "$path_to_solution")

# Извлечение числовой части из имени папки
folder_number=$(echo "$folder_name" | grep -o '[0-9]\+')

# Создание целевой папки "task<число>"
target_folder="task${folder_number}"
mkdir -p "$target_folder"

# Скачивание файла
wget -O "${target_folder}/${file_name}" "https://raw.githubusercontent.com/vkinsu/2393${i_group}/refs/heads/main/${username}/${path_to_solution}"

# Проверка успешности скачивания
if [ $? -ne 0 ]; then
    echo "Ошибка: файл не удалось скачать. Проверьте введённые параметры."
    exit 1
fi

# Вывод сообщения об успешном сохранении
echo_green "Файл сохранён в ${target_folder}/${file_name}"


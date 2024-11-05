#!/bin/bash

# Проверяем, что аргумент передан
if [ -z "$1" ]; then
  echo "Ошибка: укажите номер папки."
  exit 1
fi

# Путь к папке
folder="task$1"

# Проверяем, существует ли папка
if [ ! -d "$folder" ]; then
  echo "Ошибка: папка '$folder' не найдена."
  exit 1
fi

# Компилируем, не переходя в директорию
make -C "$folder"

# Ищем единственный запускаемый файл в папке
execute_file=$(find "$folder" -type f -executable | head -n 1)

echo -e "\n\n"
if [[ -n "$execute_file" ]]; then
    ./"$execute_file"
else
    echo "Исполняемый файл не найден в папке '$folder'."
fi


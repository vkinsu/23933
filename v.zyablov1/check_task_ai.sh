#!/bin/bash

# Проверяем, что аргумент передан

if [ -z "$1" ]; then
  echo "Ошибка: укажите номер папки."
  exit 1
fi

# Проверяем наличие флага --no-clean
clean=1
if [[ "$2" == "--no-clean" ]]; then
  clean=0
  # Сохраняем все параметры после второго аргумента как аргументы для программы
  program_args=("${@:3}")
else
  # Сохраняем все параметры начиная со второго аргумента как аргументы для программы
  program_args=("${@:2}")
fi

# Путь к папке
folder="task$1"

# Проверяем, существует ли папка
if [ ! -d "$folder" ]; then
  echo "Ошибка: папка '$folder' не найдена."
  exit 1
fi


cd $folder
make #-C "$folder"

# (вариант) Компилируем, не переходя в директорию
# make -C "$folder"

# Проверяем, что компиляция прошла успешно
if [ $? -ne 0 ]; then
  echo "Ошибка: не удалось скомпилировать в '$folder'."
  exit 1
fi


# Ищем единственный запускаемый файл в папке
execute_file=$(find . -type f -executable | head -n 1)

echo -e "\n\n"
if [[ -n "$execute_file" ]]; then
    ./"$execute_file" "${program_args[@]}"
	if [ $clean -eq 1 ]; then
  		echo "Удаление скомпилированного файла..."
	  	rm "$execute_file"
	fi
else
    echo "Исполняемый файл не найден в папке '$folder'."
fi


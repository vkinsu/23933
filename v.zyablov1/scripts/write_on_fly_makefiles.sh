#!/bin/bash

prefix="task"
# Проходим по папкам от 1 до 9
for i in {1..9}; do
    # Создаем директорию, если она не существует
    mkdir -p "$prefix$i"
    cd "$prefix$i" || exit

    echo "Введите код Makefile для папки $prefix$i (нажмите Ctrl+D для завершения):"

    # Считываем код Makefile из стандартного ввода
    makefile_content=""
    while IFS= read -r line; do
        makefile_content+="$line"$'\n'
    done

    # Записываем код в Makefile
    echo -e "$makefile_content" > Makefile

    echo "Создан Makefile в папке $prefix$i"

    # Переходим обратно в корневую директорию
    cd ..
done


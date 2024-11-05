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
source_name="task$1.c"

# Проверяем, что папка существует
if [ ! -d "$folder" ]; then
  echo "Ошибка: папка '$folder' не найдена."
  exit 1
fi

cd $folder

# Для заданий 30, 31, 32
if [[ "$1" == "30" || "$1" == "31" || "$1" == "32" ]]; then
  server_file=$(find . -type f -name "server.c" | head -n 1)
  client_file=$(find . -type f -name "client.c" | head -n 1)

  # Проверяем, что оба файла найдены
  if [ -z "$server_file" ] || [ -z "$client_file" ]; then
    echo "Ошибка: не найдены server.c и/или client.c в папке '$folder'."
    exit 1
  fi

  # Компилируем сервер
  server_exec="server_executable.out"
  gcc "$server_file" -o "$server_exec"
  if [ $? -ne 0 ]; then
    echo "Ошибка: не удалось скомпилировать файл '$server_file'."
    exit 1
  fi

  # Компилируем клиент
  client_exec="client_executable.out"
  gcc "$client_file" -o "$client_exec"
  if [ $? -ne 0 ]; then
    echo "Ошибка: не удалось скомпилировать файл '$client_file'."
    exit 1
  fi

  # Запуск сервера
  echo "Запуск сервера..."
  "./$server_exec" &
  server_pid=$!

  sleep 1 # Небольшая задержка, чтобы сервер успел запуститься

  echo "Запуск клиента..."
  "./$client_exec" "${program_args[@]}"

  # Останавливаем сервер
  kill $server_pid

  # Удаление скомпилированных файлов (для сервер-клиентных задач)
	if [ $clean -eq 1 ]; then
	  echo "Удаление скомпилированных файлов..."
	  rm "$server_exec" "$client_exec"
	fi


  exit 0
fi

# Если задание не 30, 31, или 32, продолжаем с task$1.c
c_file=$(find . -type f -name $source_name | head -n 1)

# Проверяем, что файл найден
if [ -z "$c_file" ]; then
  echo "Ошибка: .c файл не найден в папке '$folder'."
  exit 1
fi

# Компиляция файла
exec_name="task${1}_executable.out"
gcc "$c_file" -o "$exec_name"

# Проверяем, что компиляция прошла успешно
if [ $? -ne 0 ]; then
  echo "Ошибка: не удалось скомпилировать файл '$c_file'."
  exit 1
fi

# Запуск программы
"./$exec_name" "${program_args[@]}"

# Удаление скомпилированного файла (для остальных задач)
if [ $clean -eq 1 ]; then
  echo "Удаление скомпилированного файла..."
  rm "$exec_name"
fi



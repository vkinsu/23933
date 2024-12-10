# Описание
Репозиторий с решениями задач для "солярки"[^1] по дисциплине "Операционные системы" НГУ в 2024 году.

[^1]:*Linux mephisto 3.16.0-10-686-pae #1 SMP Debian 3.16.81-1 (2020-01-17) i686 GNU/Linux*

# Использование
Для каждой выполненной задачи с номером `$i` есть папка `task$i`, содержащая:
 + исходный код
 + Makefile

Некоторые задачи требуют стороннего пользователя. Для его создания воспользуйтесь командой:
```bash
echo "Enter new user's name:"; read username; sudo useradd -m "$username"
```
Чтобы очистить проект от собранных запускаемых файлов можете выполнить:
```
./scripts/delete_executables.sh
```

# Источники
[Mikhail_Vays](https://github.com/vkinsu/23930/tree/main/Mikhail_Vays): 9,10, 17,18,21,25   
[kireev](https://github.com/vkinsu/23930/tree/main/kireev): 8   
[Chat GPT](https://chatgpt.com): 2   
[a.tishkin1](https://github.com/vkinsu/23933/tree/main/a.tishkin1): 1,3,4,5,6,7, 30,31,32    
(Они не соглашались)

# Как использовать решения одногруппников
1. Находим донора: имя пользователя `${username}` и окончание номера его группы `${i_group}`.
2. Смотрим на GitHub путь к решению `"${folder_name}/${file_name}"`.
3. Применяем скрипт для скачивания
```
./scripts/download_solution.sh ${i_group} ${username} "${folder_name}/${file_name}"
```
Например для url-пути `https://github.com/vkinsu/23930/blob/main/Mikhail_Vays/task17/task17.c` такие параметры:
```
./scripts/download_solution.sh 0 Mikhail_Vays "task17/task17.c"
```

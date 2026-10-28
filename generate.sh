#!/bin/bash
# Drukuje dane w takiej kolejności, w jakiej program ich oczekuje:
# opcja 1 -> nazwa -> port -> RAM

# nazwa;port;typ;cel  (cel = nazwa kontenera albo nazwa procesu)
USLUGI='EasyConv;8010;docker;easyconv-web-1
Minecraft;25565;docker;mc-beta-mc-beta-1
Planowiec;8009;docker;planowiec-app-1
Koloseum;5001;docker;koloseum-web
HomeLibrary;8007;docker;home-library-web-1
Zflix;5002;docker;zflix-web-1
GamePlan;5005;docker;gameplan-web
NoteMaster;8008;docker;notemaster_web
TV Panel;8090;docker;tvpanel
Vmka1;8001;docker;vmportal-flask
Auth;8011;docker;hub-web
Gunicorn;8000;host;gunicorn
Nginx;80;host;nginx
SSH;22;host;sshd
PostgreSQL;5432;host;postgres
Ollama;11434;host;ollama
CUPS;631;host;cupsd'

# jedno wywolanie docker stats (kazde trwa ok. 2 s, wiec nie w petli)
STATS=$(docker stats --no-stream --format '{{.Name}} {{.MemUsage}}' | awk '{print $1, $2}')

echo "$USLUGI" | while IFS=';' read -r nazwa port typ cel; do
    if [ "$typ" = "docker" ]; then
        ram=$(echo "$STATS" | awk -v n="$cel" '$1==n {
            v=$2
            if (v ~ /GiB/) {sub(/GiB.*/,"",v); v*=1024}
            else if (v ~ /KiB/) {sub(/KiB.*/,"",v); v/=1024}
            else {sub(/MiB.*/,"",v)}
            printf "%.1f", v }')
    else
        # suma RSS (w KB) wszystkich procesow o tej nazwie -> MB
        ram=$(ps -C "$cel" -o rss= | awk '{s+=$1} END {printf "%.1f", s/1024}')
    fi
    # zabezpieczenie: program wymaga RAM > 0
    if [ -z "$ram" ] || [ "$ram" = "0.0" ]; then ram=1; fi

    echo 1
    echo "$nazwa"
    echo "$port"
    echo "$ram"
done

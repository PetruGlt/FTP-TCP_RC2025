#!/bin/bash

# Verificăm dacă a fost furnizat un director ca argument
if [ $# -eq 0 ]; then
    directory="$HOME"  # Dacă nu, folosim directorul home
else
    directory="$1"
fi

# Directorul pentru link-uri și playlist
html_dir="$HOME/html"
music_dir="$HOME/html/muzica"

# Creăm directorul pentru muzică dacă nu există
mkdir -p "$music_dir"

# Găsim toate fișierele MP3 în directorul dat și subdirectoarele lui
find "$directory" -type f -name "*.mp3" | while read -r mp3_file; do
    # Obținem numele de bază al fișierului (fără extensie)
    base_name=$(basename "$mp3_file" .mp3)
    
    # Creăm link hard în directorul de muzică
    ln "$mp3_file" "$music_dir/$base_name.mp3"
done

# Generăm playlist-ul HTML
playlist_file="$html_dir/playlist.html"
echo "<html><body><ul>" > "$playlist_file"
find "$music_dir" -type f -name "*.mp3" | while read -r mp3_file; do
    base_name=$(basename "$mp3_file" .mp3)
    echo "<li><a href=\"muzica/$base_name.mp3\">$base_name</a></li>" >> "$playlist_file"
done
echo "</ul></body></html>" >> "$playlist_file"

# Setăm permisiunile pentru directoare și fișiere
chmod -R o+x "$HOME" "$html_dir" "$music_dir"
chmod -R o+r "$playlist_file"

echo "Playlist-ul a fost creat în $playlist_file."

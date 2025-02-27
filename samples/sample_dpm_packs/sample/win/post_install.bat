:: post_install.bat

@echo off
setlocal

:: Sanal ortam oluştur
python -m venv "C:\path\to\your\project\.venv"

:: Gerekli kütüphaneleri yükle
call "C:\path\to\your\project\.venv\Scripts\python.exe" -m pip install -r "C:\path\to\your\project\requirements.txt"

exit /b 0

@echo off
title SOVOPT Launcher - Smart India Hackathon 2026
color 0b
echo ============================================================
echo   SOVOPT - Sovereign Mathematical Optimization Platform
echo   Smart India Hackathon 2026 (PS ID: 26119)
echo ============================================================
echo.
echo [1/2] Starting FastAPI Backend on http://127.0.0.1:8000 ...
start "SOVOPT Backend Server" cmd /k "cd /d %~dp0backend && python main.py"

echo [2/2] Starting Vite Frontend on http://localhost:5173 ...
start "SOVOPT Frontend App" cmd /k "cd /d %~dp0frontend && npm run dev"

echo.
echo ============================================================
echo   SOVOPT is now running!
echo   Open your browser at: http://localhost:5173
echo ============================================================
timeout /t 5


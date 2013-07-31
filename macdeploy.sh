app_path=release/Pencil.app
qt_path=/usr/local/Cellar/qt/4.7.4 # Use the path where QT is installed on your machine
verbose_level=1 # 0 = no output, 1 = error/warning (default), 2 = normal, 3 = debug

$qt_path/bin/macdeployqt $app_path -verbose=$verbose_level -dmg
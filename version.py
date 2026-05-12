
import os
import datetime

files = ['platformio.ini']

for file in os.listdir('src'):
    files.append(os.path.join('src', file))

for file in os.listdir('include'):
    if file == 'version.h':
        continue
    files.append(os.path.join('include', file))

timestamp = 0

for file in files:
    try:
        mtime = os.path.getmtime(file)
    except:
        continue
    if timestamp < mtime:
        timestamp = mtime

if timestamp > 0:
    version = datetime.datetime.fromtimestamp(timestamp)
else:
    version = datetime.datetime.now()

versionText = '\n#define BUILD_DATETIME "{:%Y-%m-%d %H:%M:%S}"\n'.format(version) + '#define BUILD_TIMESTAMP ' + str(int(timestamp)) + '\n'

with open(os.path.join('include', 'version.h'), 'r') as F:
    currentText = F.read()
    if versionText != currentText:
        with open(os.path.join('include', 'version.h'), 'w') as F:
            F.write(versionText)

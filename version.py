
import os.path
import datetime

with open(os.path.join('include', 'version.h'), 'w') as F:
    F.write('#define BUILD_DATETIME "{:%Y-%m-%d %H:%M:%S}"'.format(datetime.datetime.now()))

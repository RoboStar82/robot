
import os.path
import datetime

with open(os.path.join('include', 'version.h'), 'w') as F:
    now = datetime.datetime.now()
    F.write('\n')
    F.write('#define BUILD_DATETIME "{:%Y-%m-%d %H:%M:%S}"\n'.format(now))
    F.write('#define BUILD_TIMESTAMP {:%s}\n'.format(now))
    F.close()

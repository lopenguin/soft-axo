import shutil, os
ans = input('Are you sure? This is irreversible. (y/n) ')
if ans == 'y':
    shutil.rmtree('data')
    os.mkdir('data')
    print('data has been purged')
else:
    print('aborted')
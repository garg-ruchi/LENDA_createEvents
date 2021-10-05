import os.path
from os import path
from sys import argv

os.system("git add -A")
print('Commit message : ')
message = input()
os.system('git commit -m \"{}\"'.format(message))
os.system("git push")

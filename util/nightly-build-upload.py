#!/usr/bin/env python3

# This script is used to upload nightly builds to Google Drive.
# To use it, a service account with access to the target directory in Google
# Drive is required and the environment variable GOOGLE_PRIVATE_KEY must
# contain the private key of the service account in JSON format.

# Google Drive API v3 Reference:
# https://developers.google.com/resources/api-libraries/documentation/drive/v3/python/latest/index.html

import json
import os
import sys
import datetime

from apiclient import discovery
from apiclient.http import MediaFileUpload
from httplib2 import Http
from oauth2client.service_account import ServiceAccountCredentials

private_key = os.getenv('GOOGLE_PRIVATE_KEY')
if private_key == None:
  print("Error: Private key not set")
  sys.exit(1)

if len(sys.argv) != 3:
  print("Usage: %s [parent] [file]" % sys.argv[0])
  sys.exit(1)
[parent, file] = sys.argv[1:]

# build google drive service
scopes = ['https://www.googleapis.com/auth/drive']
keyfile_dict = json.loads(private_key)
credentials = ServiceAccountCredentials.from_json_keyfile_dict(keyfile_dict, scopes=scopes)
http = credentials.authorize(Http())
drive_service = discovery.build('drive', 'v3', http=http)

# upload nightly build
file_metadata = { 'name' : os.path.basename(file), 'parents': [parent] }
media = MediaFileUpload(file)
file = drive_service.files().create(body=file_metadata, media_body=media).execute()

# delete files older than 90 days
check_date = datetime.datetime.now() + datetime.timedelta(-90)
filter = "name contains 'pencil2d'"
filter += " and createdTime < '{}'".format(check_date.strftime("%Y-%m-%dT%H:%M:%S"))

print('filter=', filter)
print('List files older than 90 days')

response = drive_service.files().list(q=filter, spaces='drive').execute()

for f in response.get('files'):

  s = f.get('name').split('.')[0]
  s = s.split('-')
  print('-------')
  print('Deleting...')
  print("File ID:", f.get('id'))
  print("File Name:", f.get('name'))
  drive_service.files().delete(fileId=f.get('id')).execute()

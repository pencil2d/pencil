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

# Build google drive service
scopes = ['https://www.googleapis.com/auth/drive']
keyfile_dict = json.loads(private_key)
credentials = ServiceAccountCredentials.from_json_keyfile_dict(keyfile_dict, scopes=scopes)
http = credentials.authorize(Http())
drive_service = discovery.build('drive', 'v3', http=http)

# Check if file already exits (ie. there is a previous build for this PR)
response = drive_service.files().list(q="name = '%s'" % os.path.basename(file), spaces='drive').execute()
files = response.get("files")
media = MediaFileUpload(file)
if files:
    # Upload pull request build as revision
    response = drive_service.files().update(fileId=files[0].get("id"), media_body=media).execute()
else:
    # Upload pull request build as new file
    file_metadata = { 'name' : os.path.basename(file), 'parents': [parent] }
    response = drive_service.files().create(body=file_metadata, media_body=media).execute()

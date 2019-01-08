#!/usr/bin/env python3

import requests
import os
import sys
 
hook_url = 'https://oettig.xyz/api/messages'
hook_sender_id = 'BioInf Cluster Notifier'
hook_recipient_ids = {
    'peet': '1552965b-ae22-4a54-9f26-2b9ebda4a65d',
    'lukas': '',
    'group': '9bf8429c-bcc3-4baf-8d83-798cf5b9a07d'
}
 
def send_notification(text, recipient):
    json_data = {
        'recipient_token': hook_recipient_ids[recipient],
        'origin': hook_sender_id,
        'text': text
    }
    requests.post(hook_url, json=json_data)

if len(sys.argv) == 1:
    print("No message defined!")
    exit(1)

if len(sys.argv) == 2:
    send_notification(sys.argv[1], "peet")

for i in range(2, len(sys.argv)):
    if not sys.argv[i].lower() in hook_recipient_ids.keys():
        continue

    send_notification(sys.argv[1], sys.argv[i].lower())

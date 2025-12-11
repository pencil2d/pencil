#!/usr/bin/env python3
"""
Simple Transifex Translation Puller for Pencil2D

Downloads all translation files from Transifex for the Pencil2D project.

Usage:
    python3 transifex_pull.py <API_TOKEN>
    
    Or set the token as an environment variable:
    export TRANSIFEX_TOKEN='your_token'
    python3 transifex_pull.py

Setup:
    1. Get your Transifex API token from: https://app.transifex.com/user/settings/api/
    2. Run this script with the token as the first argument
    3. Translation files will be saved to the translations/ directory

Example:
    python3 transifex_pull.py 1/abc123def456...
"""

import os
import sys
import requests
import time
from pathlib import Path

def print_help():
    print(__doc__)
    sys.exit(0)

def pull_translations():
    # Show help if requested
    if len(sys.argv) > 1 and sys.argv[1] in ['-h', '--help', 'help']:
        print_help()
    
    # Get token from command line argument or environment
    if len(sys.argv) > 1:
        token = sys.argv[1]
    else:
        token = os.environ.get("TRANSIFEX_TOKEN")
    
    if not token:
        print("Error: No API token provided\n")
        print_help()
    
    # Configuration
    organization_slug = "pencil2d"
    project_slug = "pencil2d"
    resource_slug = "pencil-ts-master"
    output_dir = Path(__file__).parent.parent / "translations"
    
    # Setup headers
    headers = {
        "Authorization": f"Bearer {token}",
        "Content-Type": "application/json"
    }
    
    print("Fetching Pencil2D translations from Transifex...")
    print(f"Output: {output_dir}\n")
    
    try:
        # Format organization ID
        org_id = f"o:{organization_slug}" if not organization_slug.startswith("o:") else organization_slug
        
        # Get project
        resp = requests.get(
            f"https://rest.api.transifex.com/projects",
            params={"filter[organization]": org_id, "filter[slug]": project_slug},
            headers=headers
        )
        resp.raise_for_status()
        project_id = resp.json()["data"][0]["id"]
        
        # Get resource
        resp = requests.get(
            f"https://rest.api.transifex.com/resources",
            params={"filter[project]": project_id, "filter[slug]": resource_slug},
            headers=headers
        )
        resp.raise_for_status()
        resource_id = resp.json()["data"][0]["id"]
        
        # Get languages
        resp = requests.get(
            f"https://rest.api.transifex.com/projects/{project_id}/languages",
            params={"page[size]": 200},
            headers=headers
        )
        resp.raise_for_status()
        languages = resp.json()["data"]
        
        output_dir.mkdir(parents=True, exist_ok=True)
        
        # Download each translation
        for lang in languages:
            lang_code = lang["attributes"]["code"]
            lang_name = lang["attributes"]["name"]
            
            # Extract language ID from relationships
            language_id = lang.get("relationships", {}).get("language", {}).get("data", {}).get("id")
            if not language_id:
                language_id = lang["id"]
            
            print(f"Downloading {lang_name:<25} ({lang_code:<5})", end=" ... ")
            
            try:
                # Create async download job
                download_payload = {
                    "data": {
                        "relationships": {
                            "resource": {"data": {"type": "resources", "id": resource_id}},
                            "language": {"data": {"type": "languages", "id": language_id}}
                        },
                        "type": "resource_translations_async_downloads"
                    }
                }
                
                resp = requests.post(
                    "https://rest.api.transifex.com/resource_translations_async_downloads",
                    json=download_payload,
                    headers={**headers, "Content-Type": "application/vnd.api+json"}
                )
                resp.raise_for_status()
                job_id = resp.json()["data"]["id"]
                
                # Poll for download to complete (max 30 attempts, 2 sec intervals)
                download_url = None
                for _ in range(30):
                    resp = requests.get(
                        f"https://rest.api.transifex.com/resource_translations_async_downloads/{job_id}",
                        headers=headers,
                        allow_redirects=False
                    )
                    
                    if resp.status_code == 303:
                        download_url = resp.headers.get('Location')
                        break
                    
                    time.sleep(2)
                
                if not download_url:
                    print("TIMEOUT")
                    continue
                
                # Download the translation file
                resp = requests.get(download_url)
                resp.raise_for_status()
                
                # Save file
                filename = "pencil.ts" if lang_code == "en" else f"pencil_{lang_code}.ts"
                output_file = output_dir / filename
                output_file.write_text(resp.text, encoding="utf-8")
                print("OK")
                
            except Exception as e:
                print(f"ERROR: {e}")
        
        print(f"\nDone! Translations saved to: {output_dir}")
        
    except requests.exceptions.HTTPError as e:
        if e.response.status_code == 401:
            print("Error: Invalid or expired token")
        elif e.response.status_code == 404:
            print("Error: Project or resource not found")
        else:
            print(f"Error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    pull_translations()

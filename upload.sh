#!/bin/bash
scp -r bin dafang@student.nestbox.live:/tmp
ssh dafang@student.nestbox.live '/home/irondust/upload.sh'
#!/usr/bin/env/python
# Software License Agreement (BSD License)
#
# Copyright (c) 2008, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of the Willow Garage nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# Author: Vincent Rabaud

from document import Frame, Session
from object_db import ObjectDB
from optparse import OptionParser
import os
import subprocess

########################################################################################################################

if __name__ == '__main__':
    parser = OptionParser(description='Insert all images into the db from a specific folder.')
    parser.add_option("-p", "--training_path", dest="training_path", help="The path to all the training bags",
                      metavar="PATH_TO_IMAGES", default='./')
    parser.add_option("-m", "--meta_info", dest="meta_info", help="The meta info for the given object.",
                      metavar="META_INFO_JSON_STRING", default="Images from training path.")

    (options, args) = parser.parse_args()
    print "Training directory is %s" % options.training_path
    print "Meta info is %s" % options.meta_info

    object_db = ObjectDB()

    # process every bag file
    print 'Processing path %s' % options.training_path
    for bag_file_name in sorted(os.listdir(options.training_path)):
        bag_file_path = os.path.join(options.training_path, bag_file_name)
        if bag_file_name.endswith('.bag') and os.path.isfile(bag_file_path):
            print '* Processing bag %s' % bag_file_name

            # create the temporary folder where we will dump data
            tmp_path = os.path.join(options.training_path, '.' + bag_file_name)
            if not os.path.isdir(tmp_path):
                os.makedirs(tmp_path)

            session = Session(object_db, meta_info=options.meta_info)

            # uncompress the file
            print 'rosrun tod_training bag_dumper -B%s -P%s -N%s --image image_mono --points2 points' %\
                             (bag_file_path, tmp_path, bag_file_name)
            process = subprocess.Popen('rosrun tod_training bag_dumper -B%s -P%s -N%s --image image_mono --points2 points' %
                             (bag_file_path, tmp_path, bag_file_name), shell=True)
            process.wait()

            # store all the frames
            bag_folder = os.path.join(tmp_path, bag_file_name)
            for image_file_name in os.listdir(bag_folder):
                image_file_path = os.path.join(bag_folder, image_file_name)
                if (not image_file_name.endswith('.png')) or not os.path.isfile(image_file_path):
                    continue
                print '** Processing image %s' % image_file_name
                frame = Frame(object_db, session)
                frame.set_image(image_file_path)
                frame.persist()

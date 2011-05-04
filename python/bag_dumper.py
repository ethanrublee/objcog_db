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
import os

from optparse import OptionParser

########################################################################################################################

if __name__ == '__main__':
    parser = OptionParser(description='Insert all images into the db from a specific folder.')
    parser.add_option("-p", "--training_path", dest="training_path",
                  help="The path to all the training bags", metavar="PATH_TO_IMAGES",
                   default='./')
    parser.add_option("-e", "--extension", dest="extension",
                  help="The extension", metavar="IMAGE_TYPE_EXTENSION",
                   default='.png')
    parser.add_option("-m", "--meta_info", dest="meta_info",
                  help="The meta info for the given object.", metavar="META_INFO_JSON_STRING",
                   default="Images from training path.")

    (options, args) = parser.parse_args()
    print "Training directory is %s" % options.training_path
    print "Meta info is %s" % options.meta_info
    idx = 0

    object_db = ObjectDB()
    session = Session(object_db, meta_info=options.meta_info)

    for file in sorted(os.listdir(options.training_path)):
        if(file.endswith(options.extension)):
            print "putting %s" % file
            frame = Frame(session)
            frame.set_image(file)
            frame.persist()

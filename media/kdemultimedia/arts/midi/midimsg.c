
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "midimsg.h"

#define STATUS_MASK 0x80
#define MESSAGE_TYPE_MASK 0xf0
#define CHANNEL_MASK 0x0f

Byte midiReadByte(int fd)
{
   Byte current_byte;
   /*
   ** for now ignore all realtime-messages (0xF8 .. 0xFF), which may get
   ** embedded *inside* every other message
   */
   do {
      /* 
      ** read() is wrapped in a while() in order to handle interruption
      ** by a signal.  In the normal case, read() is only called once.
      */
      while (read(/* fd = */ fd, /* buf = */ (void *)(&current_byte), 
         /* count = */ 1) < 1) {}

   } while(current_byte >= 0xf8);

   return current_byte;
}

int midimsgGetMessageType(Byte *message)
{
   return (message[0] & MESSAGE_TYPE_MASK);
}

void midimsgSetMessageType(Byte *message_inout, int message_type)
{
   message_inout[0] = (message_inout[0] & ~MESSAGE_TYPE_MASK) | message_type;
}

int midimsgGetChannel(Byte *message)
{
   return (message[0] & CHANNEL_MASK);
}

void midimsgSetChannel(Byte *message_inout, int channel)
{
   message_inout[0] = (message_inout[0] & ~CHANNEL_MASK) | channel;
}

int midimsgGetPitch(Byte *message)
{
   return (message[1]);
}

void midimsgSetPitch(Byte *message_inout, int pitch)
{
   message_inout[1] = pitch;
}

int midimsgGetVelocity(Byte *message)
{
   return (message[2]);
}

void midimsgSetVelocity(Byte *message_inout, int velocity)
{
   message_inout[2] = velocity;
}

int midimsgGetParameterNumber(Byte *message)
{
   return (message[1]);
}

void midimsgSetParameterNumber(Byte *message_inout, int number)
{
   message_inout[1] = number;
}

int midimsgGetParameterValue(Byte *message)
{
   return (message[2]);
}

void midimsgSetParameterValue(Byte *message_inout, int value)
{
   message_inout[2] = value;
}

int midimsgGetPitchWheelValue(Byte *message)
{
   return (((int)(message[2]) << 7) + message[1]);
}

void midimsgRead(int fd, Byte *message_return)
{
   Byte current_byte;
   static Byte last_status_byte;

   current_byte = midiReadByte(fd);

   if ((current_byte & STATUS_MASK) == 0)
   {
      /* 
      ** must be a running status, unless we're picking up mid-message 
      ** (which would be an unhandled error)
      */

      message_return[0] = last_status_byte;
   }
   else
   {
      message_return[0] = current_byte;
      last_status_byte = current_byte;

      current_byte = midiReadByte(fd);
   }

   switch (midimsgGetMessageType(/* message = */ message_return))
   {
      case MIDIMSG_NOTE_ON:
      case MIDIMSG_NOTE_OFF:
      case MIDIMSG_KEY_PRESSURE:
      case MIDIMSG_PARAMETER:
      case MIDIMSG_PITCH_WHEEL:

         message_return[1] = current_byte;

         current_byte = midiReadByte(fd);
         message_return[2] = current_byte;

         if ((midimsgGetMessageType(/* message = */ message_return) == 
            MIDIMSG_NOTE_ON) && (midimsgGetVelocity(/* message = */ 
            message_return) == 0))
         {
            /* note-on with velocity of zero is equivalent to note-off */

            midimsgSetMessageType(/* message_inout = */ message_return, 
               /* message_type = */ MIDIMSG_NOTE_OFF);
         }

         return;

      case MIDIMSG_PROGRAM:
      case MIDIMSG_CHANNEL_PRESSURE:

         message_return[1] = current_byte;

         return;
   }
}

void midimsgWrite(int fd, Byte *message)
{
   switch (midimsgGetMessageType(/* message = */ message))
   {
      case MIDIMSG_NOTE_ON:
      case MIDIMSG_NOTE_OFF:
      case MIDIMSG_KEY_PRESSURE:
      case MIDIMSG_PARAMETER:
      case MIDIMSG_PITCH_WHEEL:
         write(fd, message, 3);
         break;
      case MIDIMSG_PROGRAM:
      case MIDIMSG_CHANNEL_PRESSURE:
         write(fd, message, 2);
         break;
   }
}


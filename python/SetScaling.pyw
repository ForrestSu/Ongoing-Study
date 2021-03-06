# -*- coding: utf-8 -*-

# 2014-10-29T18:58+08:00

'''Set the graphics drivesrs' scaling value on Windows NT.
For example, setting the value of 'Scaling' under:
HKLM\SYSTEM\ControlSet001\Control\GraphicsDrivers\Configuration
to 3 will make it possible to play DotA in full screen mode.'''

def SetScaling(scaling):
    import logging
    import os
    
    logging.basicConfig(level = logging.NOTSET,
                        filename = os.path.join(os.path.dirname(__file__), 'SetScaling.log'),
                        format = '%(asctime)s [%(levelname)s]: %(message)s')
    logger = logging.getLogger(__name__)
    logger.info(79 * '-')
    
    if os.name != 'nt':
        logger.error('This program can only run on Windows NT!')
        return

    try:
        import easygui
    except ImportError:
        logger.error('"easygui" is required. Please install this package.')
        return
    
    import winreg

    import RegKey

    key = winreg.HKEY_LOCAL_MACHINE
    sub_key = r'SYSTEM\ControlSet001\Control\GraphicsDrivers\Configuration'
    try:
        search_res = next(RegKey.SearchValue(key, sub_key, 'Scaling', True))
        if search_res[2] == scaling:
            easygui.msgbox('The value of "Scaling" is already {}.'
                           .format(scaling), 'Information')
            return
    except StopIteration:
        easygui.msgbox('Could not find a value named "Scaling" under "{}"!'
                       .format(sub_key), 'Warning')
        return

    sub_key = os.path.join(sub_key, search_res[0])
    if easygui.ccbox('Key: {0}\nValue name: Scaling\nData: {1}\nType: {2}\n' \
                     'Do you REALLY want to change its value from {3} to {4}?'
                     .format(sub_key, search_res[2], search_res[3],
                             search_res[2], scaling), 'Information'):
        try:
            logger.info('Try to change the value of "Scaling" from {0} to {1} ' \
                         'under key "{2}".'.format(search_res[2], scaling, sub_key))
            handle = winreg.OpenKeyEx(key, sub_key, access = winreg.KEY_SET_VALUE)
            winreg.SetValueEx(handle, 'Scaling', 0, winreg.REG_DWORD, scaling)
            winreg.CloseKey(handle)
            logger.info('Done successfully!')
            easygui.msgbox('Done!', 'Information')
        except OSError as e:
            easygui.msgbox('Failed to change the value of "Scaling"!\n' \
                           'Maybe adminstrator privilege is required.')
            logger.error('*** Failed! {!r}'.format(e))
        
if __name__ == '__main__':
    import sys

    if len(sys.argv) == 2:
        SetScaling(int(sys.argv[1]))
    else:
        SetScaling(3)

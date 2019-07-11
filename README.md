# GedTools

<https://ouuuuch.phoenixteam.org/gedtools/>

## License

The GedTools source code is distributed under the GPLv3 license.
This project also makes use of other open-source components.
Please see the accompanying `LICENSE` file for more information.

## Building

### Obtaining the Qt SDK

GedTools was built using Qt by Nokia version 4.7.3.
The Qt SDK 1.1 (with Qt 4.7.3) can be obtained from the following wiki page:

<https://wiki.qt.io/Qt_4_versions>

The links on the above wiki page currently point to platform-specific
binary installers, suffixed with `v1_1`, hosted on the following FTP mirror:

<ftp://ftp.informatik.hu-berlin.de/pub/Linux/Qt/QT/qtsdk/>

### Compiling the Project

To compile GedTools using the Qt Creator IDE, simply load the project using the
provided `GedTools.pro` file, and then use the Build command in the IDE.

To build from the command-line, first run `qmake` to generate a custom makefile
configured for your specific platform:

    qmake -makefile -o Makefile GedTools.pro

You can then use the generated makefile with `make` to compile the project:

    make

### Compiling the User Interface Translations

We use the Qt Linguist tools to provide a Traditional Chinese (Taiwan)
translation for the GedTools graphical user interface. To compile the
translation files to the runtime binary format, you need to run `lrelease`:

    lrelease GedTools.pro

### Building the Windows Installer

GedTools uses [NSIS](http://nsis.sourceforge.net/) to build its Windows
installer executable. The installer script is located in the `installer`
directory. You will need to copy the DLLs listed in the "Installer"
section of the `GedTools.nsi` script into the `installer` directory.

Depending on your build toolchain, you might need to copy a different set of
DLLs to distribute with your binary.
The [Dependency Walker](http://www.dependencywalker.com/) tool can be useful
for determining which DLLs are used by your GedTools binary.

## Usage

1. Open your PAF file (in PAF).
2. Export a GEDCOM file from PAF.
  1. Click the Export button.
  2. Make sure the GEDCOM encoding is set to UTF-8, and click Export.
  3. Save your new GEDCOM file using the Save dialog.
3. Run GedTools.exe.
4. Open the GEDCOM file that you exported from PAF.
5. From the Pinyin menu, click the Append Pinyin option.
6. From the Family Tree menu, click the Estimate Missing Dates option.
7. Save your GEDCOM file.
8. That's it! You can import your updated GEDCOM file into a new PAF file, or  import it directly into TempleReady.

## Support

Please use the GitHub issues page for GedTools to report bugs or other issues:
https://github.com/DaoWen/gedtools/issues

If you have more general questions, or if you do not have a GitHub account,
you can email the author directly:
nick.vrvilo@alumni.rice.edu

# GedTools 中文說明

## 程式使用步驟

1. 打開你的PAF檔案
2. 匯出〈GEDCOM〉的檔案:
  1. 按下〈匯出〉的按扭
  2. 確定〈GEDCOM〉已被選為匯出檔案的格式和〈UTF-8〉已被選為字元集。按下〈匯出(X)...〉，然後按下〈確定〉
  3. 儲存檔案的視窗出現時要保存這個新的〈GEDCOM〉檔案
3. 執行〈GedTools.exe〉
4. 開啓你的新〈GEDCOM〉檔案
5. 從〈拼音〉選單中選擇〈附加拼音資料〉。如果有不完整的紀錄，視窗最下面會顯示數量。
6. 從〈家族樹狀表〉選單中選擇〈推算年代〉。
7. 儲存你的〈GEDCOM〉檔案。
8. 結束了！確定一下資料都在就好了。現在可以把你剛剛儲存的〈GEDCOM〉檔案匯入到一個新的〈PAF〉檔案裡，或直接匯入到〈TempleReady〉。

## 用戶支持與聯絡方式

如果遇到困難、公式錯誤或有疑問，可以給我寫電子郵件聯絡：
nick.vrvilo@alumni.rice.edu

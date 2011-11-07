==============================================================================================================
                                                  VVASE Readme
                                                  Version 0.8b

                                               November 7, 2011

                                       Copyright Kerry R. Loux 2008-2011
==============================================================================================================

This readme must be included any time the VVASE executable or any other binaries are distributed.  It contains
copyright information not only for VVASE, but for third-party software libraries that were used in its
creation.

VVASE was compiled with Microsoft Visual C++ 2010 (Version 10.0).  If you are unable to run the
executable, you should download and install the MSVC++ 10.0 Redistributable package from Mircorosft.  It is
available for free online at:
http://www.microsoft.com/downloads/details.aspx?familyid=A5C84275-3B97-4AB7-A40D-3802B2AF5FC2&displaylang=en


Please note the disclaimer:
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Known Bugs (TODO)
-----------------
The following is a partial list of the bugs and limitations that pertain to the current version of VVASE:
- Only supports double A-Arm suspensions
- Help/documentation is so minimal that I have not bothered to include it in this release
- T-bars are not yet supported
- Third shocks/springs are not yet supported
- There is currently no way to mount the rear tie rod to anything except the tub
- There is no way for the user to interact with the plots (change the title, drag/scroll along the plot, change
  axis limits, colors, legends, etc.)
- A number of minor bugs in the way that information is displayed (names get cut off, extra white space around
  grids, etc.)
- I would like to refine the user input methods a little bit
- The toggle for View->Kinematic Analysis Toolbar isn't working quite right
- There are several areas I would like to address to increase the speed of computation
- If the pushrod or spring mounts to the upright (as opposed to one of the control arms) the motion ratio is
  not computed
- Anti-lift for FWD is not accurate
- Anti-geometries are only accurate for zero configuration (no pitch, roll, heave, or steer)
- Occasional crashes when using Genetic Algorithms



Pre-version 1.0 warning - this software is still in development.  There may be bugs that affect the stability
of the software as well as its accuracy.  If you become aware of any such problems, please let me know by
sending an e-mail to louxkr@gmail.com.  Your help is appreciated.





Third party licensing information:

wxWidgets (http://www.wxwidgets.org/)
-------------------------------------
No licensing requirement when distributing software that uses their libraries.  The libraries themselves are
licensed under the "wxWindows License."  Interested parties may read about it here:
http://www.wxwidgets.org/about/newlicen.htm
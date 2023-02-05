#pragma once

// Avoids Windows macro redefinition warnings, similar to https://developercommunity.visualstudio.com/t/warning-C4005:-Outptr:-macro-redefinit/1546919
#pragma warning( push )
#pragma warning( disable : 4005 )
#pragma warning( disable : 5106 )
import imcpp;
#pragma warning( pop )
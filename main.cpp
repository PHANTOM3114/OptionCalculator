#include <ql/qldefines.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/math/integrals/tanhsinhintegral.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanengine.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanvasicekengine.hpp>
#include <ql/pricingengines/vanilla/analytichestonengine.hpp>
#include <ql/pricingengines/vanilla/baroneadesiwhaleyengine.hpp>
#include <ql/pricingengines/vanilla/batesengine.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/pricingengines/vanilla/bjerksundstenslandengine.hpp>
#include <ql/pricingengines/vanilla/fdblackscholesvanillaengine.hpp>
#include <ql/pricingengines/vanilla/integralengine.hpp>
#include <ql/pricingengines/vanilla/mcamericanengine.hpp>
#include <ql/pricingengines/vanilla/mceuropeanengine.hpp>
#include <ql/pricingengines/vanilla/qdfpamericanengine.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>

#include <iostream>
#include <iomanip>
#include <string>
 
using namespace QuantLib;
 
int main(int, char* []) {

    std::cout << "Welcome to our Option Calculator. Enter the next necessary information" << std::endl;

    Option::Type type(Option::Put);
    Real underlying, strike;
    Spread dividendYield;
    Rate riskFreeRate;
    Volatility volatility;

    std::cout << "Enter a underlying price: ";
    std::cin >> underlying;

    if (underlying >= 100) {
        strike = 200;
        dividendYield = 0.01;
        riskFreeRate = 0.1;
        volatility = 0.50;
    }
    else {
        strike = 100;
        dividendYield = 0.00;
        riskFreeRate = 0.07;
        volatility = 0.20;
    }


    try {

        std::cout << std::endl;

        // set up dates
        Calendar calendar = TARGET();
        Date todaysDate(15, May, 2021);
        Date settlementDate(17, May, 2021);
        Settings::instance().evaluationDate() = todaysDate;

        Date maturity(17, May, 2022);
        DayCounter dayCounter = Actual365Fixed();

        std::cout << "Option type = "  << type << std::endl;
        std::cout << "Maturity = "        << maturity << std::endl;
        std::cout << "Underlying price = "        << underlying << std::endl;
        std::cout << "Strike = "                  << strike << std::endl;
        std::cout << "Risk-free interest rate = " << io::rate(riskFreeRate)
                  << std::endl;
        std::cout << "Dividend yield = " << io::rate(dividendYield)
                  << std::endl;
        std::cout << "Volatility = " << io::volatility(volatility)
                  << std::endl;
        std::cout << std::endl;
        std::string method;
        std::cout << std::endl ;

        // write column headings
        Size widths[] = { 35, 14, 14, 14 };
        std::cout << std::setw(widths[0]) << std::left << "Method"
                  << std::setw(widths[1]) << std::left << "European"
                  << std::setw(widths[2]) << std::left << "Bermudan"
                  << std::setw(widths[3]) << std::left << "American"
                  << std::endl;

        std::vector<Date> exerciseDates;
        for (Integer i=1; i<=4; i++)
            exerciseDates.push_back(settlementDate + 3*i*Months);

        auto europeanExercise = ext::make_shared<EuropeanExercise>(maturity);

        auto bermudanExercise = ext::make_shared<BermudanExercise>(exerciseDates);

        auto americanExercise = ext::make_shared<AmericanExercise>(settlementDate, maturity);

        auto underlyingH = makeQuoteHandle(underlying);

        // bootstrap the yield/dividend/vol curves
        Handle<YieldTermStructure> flatTermStructure(
            ext::make_shared<FlatForward>(settlementDate, riskFreeRate, dayCounter));
        Handle<YieldTermStructure> flatDividendTS(
            ext::make_shared<FlatForward>(settlementDate, dividendYield, dayCounter));
        Handle<BlackVolTermStructure> flatVolTS(
            ext::make_shared<BlackConstantVol>(settlementDate, calendar, volatility,
                                     dayCounter));
        auto payoff = ext::make_shared<PlainVanillaPayoff>(type, strike);
        auto bsmProcess = ext::make_shared<BlackScholesMertonProcess>(
                underlyingH, flatDividendTS, flatTermStructure, flatVolTS);

        // options
        VanillaOption europeanOption(payoff, europeanExercise);
        VanillaOption bermudanOption(payoff, bermudanExercise);
        VanillaOption americanOption(payoff, americanExercise);

        // Analytic formulas:

        // Black-Scholes for European
        method = "Black-Scholes";
        europeanOption.setPricingEngine(ext::make_shared<AnalyticEuropeanEngine>(bsmProcess));
        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::left << europeanOption.NPV()
                  << std::setw(widths[2]) << std::left << "N/A"
                  << std::setw(widths[3]) << std::left << "N/A"
                  << std::endl;

        // Barone-Adesi and Whaley approximation for American
        method = "Barone-Adesi/Whaley";
        americanOption.setPricingEngine(ext::make_shared<BaroneAdesiWhaleyApproximationEngine>(bsmProcess));
        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::left << "N/A"
                  << std::setw(widths[2]) << std::left << "N/A"
                  << std::setw(widths[3]) << std::left << americanOption.NPV()
                  << std::endl;
        return 0;

    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}

 

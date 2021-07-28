#if !defined(DQN_MONEY_H)
#define DQN_MONEY_H

#if !defined(DQN_U128_H)
    #error You must include "Dqn_U128.h" before including "Dqn_Money.h"
#endif // DQN_U128_H

struct Dqn_MoneyString
{
    char str[64];
    int  size;
};

int const DQN_MONEY_BTC_DECIMAL_PLACES    = 8;
int const DQN_MONEY_ETH_DECIMAL_PLACES    = 18;
int const DQN_MONEY_BNB_DECIMAL_PLACES    = DQN_MONEY_ETH_DECIMAL_PLACES;
int const DQN_MONEY_DOLLAR_DECIMAL_PLACES = 2;
int const DQN_MONEY_OXEN_DECIMAL_PLACES   = 9;

Dqn_MoneyString Dqn_MoneyU128ToString(intx::uint128 atomic_amount, intx::uint128 decimal_places, Dqn_b32 comma_sep = true);
#endif // DQN_MONEY_H

#if defined(DQN_MONEY_IMPLEMENTATION)
Dqn_MoneyString Dqn_MoneyU128ToString(intx::uint128 atomic_amount, intx::uint128 decimal_places, Dqn_b32 comma_sep)
{
    DQN_HARD_ASSERT_MSG(decimal_places < 32, "// TODO(dqn): Verify what our limits for this are");

    intx::uint128 atomic_units_per_currency = 1;
    for (int place = 0; place < decimal_places; place++)
        atomic_units_per_currency *= 10;

    intx::uint128   atomic_part = atomic_amount % atomic_units_per_currency;
    intx::uint128   whole_part  = atomic_amount / atomic_units_per_currency;
    Dqn_MoneyString string      = {};

    if (atomic_amount == 0)
    {
        string.str[string.size++] = '0';
        string.str[string.size++] = '.';
        string.str[string.size++] = '0';
    }
    else
    {
        int           atomic_part_size = 0;
        intx::uint128 atomic_copy      = atomic_part;

        // Skip any trailing 0s in the fractional part
        for (; atomic_copy > 0; atomic_copy /= 10, atomic_part_size++)
        {
            char digit = (char)(atomic_copy % 10);
            if (digit != 0) break;
        }

        // Write the fraction part into the string
        for (; atomic_copy > 0; atomic_copy /= 10, atomic_part_size++)
        {
            char digit                = (char)(atomic_copy % 10);
            string.str[string.size++] = '0' + digit;
        }

        // If the string has a size we have a fractional number
        if (string.size)
        {
            // Add zeros until the fractional part is complete
            for (int digits = atomic_part_size; digits < decimal_places; digits++)
                string.str[string.size++] = '0';

            string.str[string.size++] = '.';
        }

        // Write the whole part into the string
        if (whole_part == 0)
        {
            string.str[string.size++] = '0';
        }
        else
        {
            for (int digit_count = 0; whole_part > 0; digit_count++)
            {
                if (comma_sep && (digit_count != 0) && (digit_count % 3 == 0))
                    string.str[string.size++] = ',';

                char digit               = (char)(whole_part % 10);
                string.str[string.size++] = '0' + digit;
                whole_part /= 10;
            }
        }
    }

    Dqn_MoneyString result = {};
    for (int i = string.size - 1; i >= 0; i--)
        result.str[result.size++] = string.str[i];
    return result;
}
#endif // DQN_MONEY_IMPLEMENTATION

<?php

use App\Models\Appliance;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| API Routes
|--------------------------------------------------------------------------
|
| Here is where you can register API routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned to the "api" middleware group. Make something great!
|
*/

// create a toggle for the appliance1 status
Route::get('/appliance1/toggle', function () {
    $appliance = Appliance::find(1);
    $appliance->status = !$appliance->status;
    $appliance->save();

    return response()->json($appliance);
});

// create a toggle for the appliance2 status
Route::get('/appliance2/toggle', function () {
    $appliance = Appliance::find(2);
    $appliance->status = !$appliance->status;
    $appliance->save();

    return response()->json($appliance);
});

// create a route to reset all appliances
Route::get('/reset', function () {
    Appliance::query()->update(['status' => 0]);

    return response()->json('All appliances reset');
});

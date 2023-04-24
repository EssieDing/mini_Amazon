from django.contrib import admin
from .models import *
from amazon.models import Item, Order, Package, Inventory

# Register your models here.
admin.site.register(Item)
admin.site.register(Order)
admin.site.register(Package)
admin.site.register(Inventory)
admin.site.register(Profile)
